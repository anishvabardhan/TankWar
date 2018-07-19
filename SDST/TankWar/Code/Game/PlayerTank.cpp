#include "Game/PlayerTank.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/GameMap.hpp"
#include "Game/GameState/GameState_Playing.hpp"
#include "Game/Bullet.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/StopWatch.hpp"
#include "Engine/Renderer/DebugRenderUtils.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/Ray3.hpp"
#include "Engine/Math/RaycastHit3D.hpp"
#include "Engine/Audio/AudioSystem.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
PlayerTank::PlayerTank()
	: Tank()
{
	m_fireInterval = new StopWatch();
	m_fireInterval->SetTimer(0.8f);
	float radius = 0.5f;

	m_turretRenderable = new Renderable();
	m_turretTransform = new Transform();
	m_turretRenderable->SetWatchTransform(m_turretTransform);
	m_transform->AddChild(m_turretTransform);
	m_turretTransform->SetPosition(0.f, 0.5f, 0.f);
	
	MeshBuilder turretBuilder;
	turretBuilder.Begin(PRIMITIVE_TRIANGLES, true);
	turretBuilder.AddSphere(Vector3::ZERO, radius, 16, 16, Rgba::WHITE);
	turretBuilder.AddCube(Vector3(0.f, 0.2f, 1.f), Vector3(0.2f, 0.2f, 2.f));
	turretBuilder.End();
	Mesh* turretMesh = turretBuilder.CreateMesh<VertexLit>();
	m_turretRenderable->SetMesh(turretMesh);
	m_turretRenderable->SetMaterial(*Renderer::GetInstance()->CreateOrGetMaterial("Data/Materials/Tank.mat"));

	m_sphereCollider.radius = radius;
	m_sphereCollider.center = m_transform->GetWorldPosition();
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
PlayerTank::~PlayerTank()
{
	delete m_turretTransform;
	m_turretTransform = nullptr;

	delete m_turretRenderable;
	m_turretRenderable = nullptr;

	delete m_fireInterval;
	m_fireInterval = nullptr;

	delete m_camera;
	m_camera = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Initializes the player tank
//
void PlayerTank::Initialize()
{
	m_camera = new OrbitCamera();
	m_camera->SetSkyBox(Renderer::GetInstance()->CreateOrGetCubeMap("Data/Images/skybox.jpg"));
	m_camera->SetMaterial(Renderer::GetInstance()->CreateOrGetMaterial("Data/Materials/skybox.mat"));
	m_camera->SetColorTarget(Renderer::GetInstance()->m_defaultColorTarget);
	m_camera->SetDepthTarget(Renderer::GetInstance()->m_defaultDepthTarget);
	m_camera->SetPerspective(60.f, 1.f, 0.1f, 1000.f);
	DebugRenderer::GetInstance()->SetDebug3DCamera(m_camera);
	
	m_gameState = (GameState_Playing*) g_curState;
	
	m_gameInstance = Game::GetInstance();
}

//-----------------------------------------------------------------------------------------------
// Updates the player tank
//
void PlayerTank::Update(float deltaSeconds)
{
	m_gameState = (GameState_Playing*) g_curState;
	if(!IsDevConsoleOpen())
	{
		ProcessInput(deltaSeconds);
		ProcessMouseInput(deltaSeconds);
	}

	UpdateTarget();
	m_camera->SetTarget(m_transform->GetWorldPosition() + Vector3::UP * 2.f);
	m_sphereCollider.center = m_transform->GetWorldPosition();

	
	OrientTowardTarget(deltaSeconds);
	Tank::Update(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Updates the target based on Gamemap raycast
//
void PlayerTank::UpdateTarget()
{
	if(m_isDead)
	{
		return;
	}

	Ray3 ray(m_camera->m_transform.GetWorldPosition(), m_camera->m_transform.GetForward());

	RaycastHit hitResult = m_gameState->Raycast(ray, 50.f);
	if(hitResult.hit)
	{
		m_target = hitResult.position;
		DebugRenderPoint(0.f, m_target, Rgba::BLUE, Rgba::BLUE);
		DebugRenderLogf(0.f, "%s", m_target.ToString().c_str());
	}
	else
	{
		m_target = ray.Evaluate(1000.f); // Arbitrary point to say it didn't hit
	}

	DebugRenderLine(0.f, m_turretTransform->GetWorldPosition(), m_target, Rgba::RED, DEBUG_RENDER_IGNORE_DEPTH);
}

//-----------------------------------------------------------------------------------------------
// Orients the turret towards the target
//
void PlayerTank::OrientTowardTarget( float deltaSeconds )
{
	if(m_isDead)
	{
		return;
	}

	Vector3 translation = m_transform->GetWorldPosition();
	Vector3 localTarget = m_transform->GetWorldMatrix().GetInverse() * Vector4(m_target, 1.f);

	Matrix44 lookAt = Matrix44::LookAt(m_turretTransform->GetLocalPosition(), localTarget, m_transform->GetUp());
	Matrix44 current = m_turretTransform->GetLocalMatrix();

	float turnAmount = TURRET_ROTATION_SPEED * deltaSeconds;
	Matrix44 localMatrix = TurnToward(lookAt, current, turnAmount);
	m_turretTransform->SetLocalMatrix(localMatrix);
}

//-----------------------------------------------------------------------------------------------
// Renders extra stuff on the tank
//
void PlayerTank::Render() const
{

}

//-----------------------------------------------------------------------------------------------
// Process keyinput for the player 
//
void PlayerTank::ProcessInput(float deltaSeconds)
{
	InputSystem* input = InputSystem::GetInstance();
	
	if(m_isDead) // Don't process keyboard input if the player is dead
		return; 

	bool movedThisFrame = false;
	// Translation controls
	if(input->IsKeyDown(KEYCODE_W))
	{
		m_transform->Translate(m_transform->GetForward() * (deltaSeconds * TANK_MOVE_SPEED));
		movedThisFrame = true;
	}
	if(input->IsKeyDown(KEYCODE_S))
	{
		m_transform->Translate(m_transform->GetForward() * (deltaSeconds * TANK_MOVE_SPEED * -1.f));
		movedThisFrame = true;
	}
	if(input->IsKeyDown(KEYCODE_A))
	{
		m_transform->RotateByEuler(0.f, deltaSeconds * TANK_ROTATION_SPEED * -1.f, 0.f);
		movedThisFrame = true;
	}
	if(input->IsKeyDown(KEYCODE_D))
	{
		m_transform->RotateByEuler(0.f, deltaSeconds * TANK_ROTATION_SPEED, 0.f);
		movedThisFrame = true;
	}
	if(input->IsKeyDown(KEYCODE_Q))
	{
		m_cameraZoom += deltaSeconds * CAMERA_ZOOM_SPEED;
	}
	if(input->IsKeyDown(KEYCODE_E))
	{
		m_cameraZoom -= deltaSeconds * CAMERA_ZOOM_SPEED;
	}
	
	if(input->IsMouseButtonDown(MOUSE_LMB))
	{
		 if(m_fireInterval->DecrementAll())
		 {
			 FireBullet();
		 }
	}

}

//-----------------------------------------------------------------------------------------------
// Processes mouse input for the player
//
void PlayerTank::ProcessMouseInput(float deltaSeconds)
{
	Vector2 mouseDelta = InputSystem::GetMouseDelta();

	m_cameraPitch += mouseDelta.y * deltaSeconds * CAMERA_ROTATION_SPEED;
	m_cameraYaw -= mouseDelta.x * deltaSeconds * CAMERA_ROTATION_SPEED; 
	m_cameraYaw = fmodf(m_cameraYaw, 360.f);
	m_cameraPitch = ClampFloat(m_cameraPitch, 0.f, 30.f);
	DebugRenderLogf(0.f, "Pitch:%0.2f", m_cameraPitch);

	m_camera->SetSphericalCoordinates(m_cameraZoom, m_cameraYaw, m_cameraPitch);
}

//-----------------------------------------------------------------------------------------------
// Spawns a bullet and adds it to the scene
//
void PlayerTank::FireBullet() const
{
	Bullet* bullet = new Bullet();
	bullet->SetPosition(m_turretTransform->GetWorldPosition());
	bullet->m_direction = m_turretTransform->GetForward();

	AudioSystem::GetInstance()->PlayOneOffGroup("player.cannon");

	m_gameState->AddBullet(bullet);
}

//-----------------------------------------------------------------------------------------------
// Take damage and mark for delete
//
void PlayerTank::TakeDamage(int damage)
{
	AudioSystem::GetInstance()->PlayOneOffGroup("enemy.hit");
	m_health -= damage;
	if(m_health <= 0)
	{
		m_gameState->DisablePlayer();
		m_isDead = true;
	}
}

