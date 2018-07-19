#include "Game/EnemyTank.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/GameMap.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/GameState/GameState_Playing.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/DebugRenderUtils.hpp"
#include "Engine/Audio/AudioSystem.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
EnemyTank::EnemyTank()
	: GameObject()
{
	m_gameState = (GameState_Playing*) g_curState;

	float radius = 1.f;

	MeshBuilder builder;
	builder.Begin(PRIMITIVE_TRIANGLES, true);
	builder.AddSphere(Vector3::ZERO, radius, 32, 32, Rgba::RED);
	builder.AddCube(Vector3(0.f, 0.f, 1.f), Vector3::ONE * 0.5f, Rgba::YELLOW);
	builder.End();
	Mesh* enemyMesh = builder.CreateMesh<VertexLit>();

	Material* enemyMat = Renderer::GetInstance()->CreateOrGetMaterial("Data/Materials/enemy.mat");
	m_renderable->SetMaterial(*enemyMat);
	m_renderable->SetMesh(enemyMesh);

	m_sphereCollider.center = Vector3::ZERO;
	m_sphereCollider.radius = radius;
}

//-----------------------------------------------------------------------------------------------
// Returns the world position from the transform
//
Vector3 EnemyTank::GetWorldPos() const
{
	return m_transform->GetWorldPosition();
}

//-----------------------------------------------------------------------------------------------
// Updates the enemy tank
//
void EnemyTank::Update(float deltaSeconds)
{
	if(!m_gameState)
	{
		m_gameState = (GameState_Playing*) g_curState;
		return;
	}

	// Adjust height based on position XZ
	Vector3 position = m_transform->GetWorldPosition();
	position.y = m_map->GetLinearHeight(position.xz()) + 0.5f;
	m_transform->SetPosition(position);

	ResetForces();
	UpdateLocalFlock();
	UpdatePlayerSeekForce();
	UpdateSeparationForce();
	UpdateCohesiveForce();
	UpdateAlignmentForce();

	m_velocity = m_seekForce + m_separationForce + m_cohesiveForce + m_alignmentForce;
	m_velocity.Normalize();
	m_velocity *= SWARM_MOVE_SPEED * deltaSeconds;

	m_transform->Translate(m_velocity);
	Matrix44 alignmentMatrix = ComputeAlignmentMatrix();
	m_transform->SetLocalMatrix(alignmentMatrix);
	OrientToVelocity(deltaSeconds);

	m_sphereCollider.center = m_transform->GetWorldPosition();

	CheckResolvePlayerCollision();
}

//-----------------------------------------------------------------------------------------------
// Renders stuff on the enemy tank
//
void EnemyTank::Render() const
{
	
}

//-----------------------------------------------------------------------------------------------
// Compute the terrain alignment matrix for the tank position
//
Matrix44 EnemyTank::ComputeAlignmentMatrix()
{
	Vector3 normal = m_map->GetNormalAtPosition(m_transform->GetWorldPosition());

	Vector3 right = CrossProduct(Vector3::UP, m_transform->GetForward());
	Vector3 correctForward = CrossProduct(right, normal);
	Vector3 correctRight = CrossProduct(normal, correctForward);

	Vector3 translation = m_transform->GetWorldPosition();

	Matrix44 mat(correctRight, normal, correctForward, translation);

	return mat;
}

//-----------------------------------------------------------------------------------------------
// Returns true if the point is inside the enemy sphere
//
bool EnemyTank::IsPointInside(const Vector3& pos)
{
	return m_sphereCollider.IsPointInside(pos);
}

//-----------------------------------------------------------------------------------------------
// Resets all forces
//
void EnemyTank::ResetForces()
{
	m_velocity = Vector3::ZERO;
	m_seekForce = Vector3::ZERO;
	m_separationForce = Vector3::ZERO;
	m_cohesiveForce = Vector3::ZERO;
	m_alignmentForce = Vector3::ZERO;
}

//-----------------------------------------------------------------------------------------------
// Updates the list of local flockmates
//
void EnemyTank::UpdateLocalFlock()
{
	m_localFlock.clear();

	float swarmRadiusSq = SWARM_LOCAL_RADIUS * SWARM_LOCAL_RADIUS;
	std::vector<EnemyTank*> enemyList =	m_gameState->m_enemies;
	for(EnemyTank* enemy : enemyList)
	{
		if(enemy == this)
		{
			continue;
		}

		Vector3 myPos = m_transform->GetWorldPosition();
		Vector3 otherPos = enemy->m_transform->GetWorldPosition();
		float distanceSq = (otherPos - myPos).GetLengthSquared();

		if(distanceSq < swarmRadiusSq)
		{
			m_localFlock.push_back(enemy);
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Updates the force to seek to the player
//
void EnemyTank::UpdatePlayerSeekForce()
{
	m_seekForce = m_gameState->m_playerTank->m_transform->GetWorldPosition() - m_transform->GetWorldPosition();
	m_seekForce.Normalize();

	m_seekForce *= SWARM_SEEK_WEIGHT;
}

//-----------------------------------------------------------------------------------------------
// Updates the force to avoid crowds with local flockmates
//
void EnemyTank::UpdateSeparationForce()
{
	if(m_localFlock.size() <= 0)
		return;

	int separationFlockCount  = 0;
	Vector3 dirToSeparate;
	for(EnemyTank* enemy : m_localFlock)
	{
		Vector3 myPos = m_transform->GetWorldPosition();
		Vector3 otherPos = enemy->m_transform->GetWorldPosition();
		float distanceSq = (otherPos - myPos).GetLengthSquared();

		if(distanceSq < (SWARM_SEPARATION_RADIUS * SWARM_SEPARATION_RADIUS))
		{

			dirToSeparate += enemy->GetWorldPos();
			separationFlockCount++;
		}
	}

	if(separationFlockCount)
	{
		dirToSeparate = dirToSeparate / (float)separationFlockCount;
		dirToSeparate = GetWorldPos() - dirToSeparate;
		dirToSeparate.Normalize();
	}
	
	m_separationForce = dirToSeparate * SWARM_SEPARATE_WEIGHT;
}

//-----------------------------------------------------------------------------------------------
// Updates the force to move towards the average position local swarm
//
void EnemyTank::UpdateCohesiveForce()
{
	if(m_localFlock.size() <= 0)
		return;

	Vector3 dirToPos;
	int cohesionForceCount = 0;
	for(EnemyTank* enemy : m_localFlock)
	{
		Vector3 myPos = m_transform->GetWorldPosition();
		Vector3 otherPos = enemy->m_transform->GetWorldPosition();
		float distanceSq = (otherPos - myPos).GetLengthSquared();

		if(distanceSq > (SWARM_COHESION_RADIUS * SWARM_COHESION_RADIUS))
		{
			dirToPos += enemy->GetWorldPos();
			cohesionForceCount++;
		}
	}

	if(cohesionForceCount)
	{
		dirToPos *= 1.f / (float)cohesionForceCount;
		dirToPos = dirToPos - GetWorldPos();
		dirToPos.Normalize();
	}

	m_cohesiveForce = dirToPos * SWARM_COHESION_WEIGHT;
}

//-----------------------------------------------------------------------------------------------
// Updates the force to align towards the local swarm's average velocity
//
void EnemyTank::UpdateAlignmentForce()
{
	if(m_localFlock.size() <= 0)
	{
		m_alignmentForce = m_velocity;
		return;
	}

	Vector3 velocityAvg;
	for(EnemyTank* enemy : m_localFlock)
	{
		velocityAvg += enemy->m_velocity;
	}
	velocityAvg *= 1.f / (float) m_localFlock.size();
	velocityAvg.Normalize();

	m_alignmentForce = velocityAvg * SWARM_ALIGNMENT_WEIGHT;
}

//-----------------------------------------------------------------------------------------------
// Orients the enemy towards the velocity vector (World matrices because there's no parent)
//
void EnemyTank::OrientToVelocity( float deltaSeconds )
{
	Vector3 target = m_transform->GetWorldPosition() + m_velocity;

	Matrix44 lookAt = Matrix44::LookAt(m_transform->GetWorldPosition(), target);

	Matrix44 current = m_transform->GetWorldMatrix();

	float turnAmount = TANK_ROTATION_SPEED * deltaSeconds;
	Matrix44 worldMatrix = TurnToward(lookAt, current, turnAmount);
	m_transform->SetWorldMatrix(worldMatrix);
}

//-----------------------------------------------------------------------------------------------
// Checks and damages player if there is collision 
//
void EnemyTank::CheckResolvePlayerCollision()
{
	PlayerTank* player = m_gameState->m_playerTank;
	if(player->m_isDead)
	{
		return;
	}

	if(DoSpheresOverlap(m_sphereCollider, player->m_sphereCollider))
	{
		AudioSystem::GetInstance()->PlayOneOffGroup("enemy.die");
		m_isReadyToDestroy = true;
		player->TakeDamage(10);
	}
}
