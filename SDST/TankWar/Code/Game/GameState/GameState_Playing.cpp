#include "Game/GameState/GameState_Playing.hpp"

//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/GameObject.hpp"
#include "Game/GameMap.hpp"
#include "Game/GameMapChunk.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/EnemyTank.hpp"
#include "Game/EnemySpawn.hpp"
#include "Game/Bullet.hpp"
#include "Game/GameState/GameState_MainMenu.hpp"

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Mesh/Mesh.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRenderUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Mesh/MeshBuilder.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Console/CommandDefinition.hpp"
#include "Engine/Console/Command.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Renderer/ParticleEmitter.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/RaycastHit3D.hpp"
#include "Engine/Math/Ray3.hpp"
#include "Engine/Core/StopWatch.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
GameState_Playing::GameState_Playing() 
	: GameState()
{
	m_renderPath = new ForwardRenderPath();
	m_scene = new RenderScene();
	m_map = new GameMap();
	m_waterRenderable = new Renderable();
	m_waterTransform = new Transform();
	m_waterRenderable->SetWatchTransform(m_waterTransform);

	InputSystem::SetMouseMode(MOUSE_RELATIVE);
	InputSystem::LockMouseToWindow(true);

	Renderer::GetInstance()->SetAmbientLight(Rgba::WHITE, 0.2f);
	CreateScene();

	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget(Renderer::GetInstance()->m_defaultColorTarget);
	m_uiCamera->SetDepthTarget(Renderer::GetInstance()->m_defaultDepthTarget);
	m_uiCamera->SetOrtho(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);

	m_respawnTimer = new StopWatch();
	m_respawnTimer->SetTimer(RESPAWN_TIME);
	m_respawnTimer->Stop();

	m_musicPlaybackID = AudioSystem::GetInstance()->PlayOneOffGroup("music.game");

	COMMAND("killall", KillAllCommand, "Kills all enemies and bases in the level");
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
GameState_Playing::~GameState_Playing()
{
	COMMANDFORGET("killall");
}

//-----------------------------------------------------------------------------------------------
// Returns the render ui bounds in normalized coords
//
AABB2 GameState_Playing::GetUIBounds() const
{
	AABB2 bounds;
	bounds.mins = Vector2(0.7f, 0.8f);
	bounds.maxs = Vector2(1.f, 1.f);

	return bounds;
}

//-----------------------------------------------------------------------------------------------
// Returns the health bar bounds in normalized coords
//
AABB2 GameState_Playing::GetHealthUIBounds() const
{
	AABB2 bounds = GetUIBounds();
	bounds.mins.y += 0.01f;
	bounds.maxs.y = bounds.mins.y + 0.05f;;

	return bounds;
}

//-----------------------------------------------------------------------------------------------
// Returns the health panel bounds in normalized coords
//
AABB2 GameState_Playing::GetEnemyUIBounds() const
{
	AABB2 bounds = GetUIBounds();
	bounds.mins.y += 0.07f;
	bounds.maxs.y = bounds.mins.y + 0.05f;

	return bounds;
}

//-----------------------------------------------------------------------------------------------
// returns the enemy base UI text bounds
//
AABB2 GameState_Playing::GetEnemyBaseUIBounds() const
{
	AABB2 bounds = GetUIBounds();
	bounds.mins.y += 0.12f;
	bounds.maxs.y = bounds.mins.y + 0.05f;

	return bounds;
}

//-----------------------------------------------------------------------------------------------
// Creates the game scene 
//
void GameState_Playing::CreateScene()
{
	Image* heightMap = new Image("Data/Images/Terrain/m6.png");
	m_map->LoadFromImage(*heightMap, AABB2(Vector2::ZERO, 128.f, 128.f), -4.f, 4.f, IntVector2(16,16) );
	//m_scene->AddRenderable(m_map->m_renderable);
	for(GameMapChunk* chunk : m_map->m_chunks)
	{
		m_scene->AddRenderable(chunk->m_renderable);
	}

	Vector3 waterPos(-128.f, -128.f);
	waterPos.y = m_map->GetLinearHeight(Vector2::ZERO);
	MeshBuilder builder;
	builder.Begin(PRIMITIVE_TRIANGLES, true);
	builder.SetColor(Rgba(255,255,255,200));
	builder.AddPlane(waterPos, Vector3::RIGHT, Vector3::FORWARD, Vector2(256.f, 256.f));
	builder.End();
	Mesh* waterPlane = builder.CreateMesh<VertexLit>();

	Material* waterMaterial = Renderer::GetInstance()->CreateOrGetMaterial("Data/Materials/water.mat");
	m_waterRenderable->SetMesh(waterPlane);
	m_waterRenderable->SetMaterial(*waterMaterial);
	m_scene->AddRenderable(m_waterRenderable);

	Light* sun = new Light();
	sun->SetupDirectionalLight(Vector3(0.f, 32.f, 0.f), Vector3(20.f, 0.f), Rgba::WHITE, 15.f);
	m_scene->AddLight(sun);

	EnemySpawn* spawner = new EnemySpawn();
	spawner->m_map = m_map;
	spawner->SetXZPosition(Vector2::ONE * 5.f);
	AddEnemySpawn(spawner);
}

//-----------------------------------------------------------------------------------------------
// Adds a particle emitter to the scene
//
void GameState_Playing::AddEmitter(ParticleEmitter* emitter)
{
	m_emitters.push_back(emitter);
	m_scene->AddRenderable(emitter->m_renderable);
}

//-----------------------------------------------------------------------------------------------
// Removes a particle emitter from the scene
//
void GameState_Playing::RemoveEmitter(ParticleEmitter* emitter)
{
	for(size_t index = 0; index < m_emitters.size(); ++index)
	{
		if(m_emitters[index] == emitter)
		{
			m_scene->RemoveRenderable(emitter->m_renderable);
			delete m_emitters[index];
			m_emitters[index] = nullptr;
			m_emitters[index] = m_emitters[m_emitters.size() - 1];
			m_emitters.pop_back();
			index--;
			
			return;
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Processes input 
//
void GameState_Playing::ProcessInput()
{
	InputSystem* input = InputSystem::GetInstance();
	Camera* gameCam = Game::GetInstance()->m_gameCamera;
	float deltaSeconds = (float) Game::GetInstance()->m_gameClock->GetDeltaSeconds();
	UNUSED(deltaSeconds);
	UNUSED(gameCam);

	if(input->WasKeyJustPressed(KEYCODE_O))
	{
		m_renderGrid = !m_renderGrid;
	}

	if(input->WasKeyJustPressed(KEYCODE_Y))
	{
		m_renderMode--;
	}

	if(input->WasKeyJustPressed(KEYCODE_U))
	{
		m_renderMode++;
	}
	m_renderMode = ClampInt(m_renderMode, 0, 7);

	if(input->WasKeyJustPressed(KEYCODE_ENTER) )
	{
		if(m_playerTank->m_isDead)
		{
			StopRespawnTimer();
			RespawnPlayer();
		}

		else if(CheckVictory())
		{
			AudioSystem::GetInstance()->StopSound(m_musicPlaybackID);
			Game::GetInstance()->ChangeState( new GameState_MainMenu() );
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Processes mouse input
//
void GameState_Playing::ProcessMouseInput()
{

}

//-----------------------------------------------------------------------------------------------
// Updates the game state
//
void GameState_Playing::Update(float deltaSeconds)
{
	if(!m_playerTank)
	{
		m_playerTank = new PlayerTank();
		m_playerTank->Initialize();
		m_playerTank->m_map = m_map;
		g_curState = this;
		AddTank(m_playerTank);
		m_scene->AddRenderable(m_playerTank->m_turretRenderable);
		m_scene->AddCamera((Camera*) m_playerTank->m_camera);;
	}

	GameState::Update(deltaSeconds);

	Renderer* rend = Renderer::GetInstance();
	
	DebugRender();

	

	for(GameObject* object : m_gameObjects)
	{
		object->Update(deltaSeconds);
	}

	for(EnemyTank* enemies : m_enemies)
	{
		enemies->Update(deltaSeconds);
	}

	for(EnemySpawn* spawner : m_bases)
	{
		spawner->Update(deltaSeconds);
	}

	for(Bullet* bullet : m_bullets)
	{
		bullet->Update(deltaSeconds);
	}

	for(ParticleEmitter* emitter : m_emitters)
	{
		emitter->Update(deltaSeconds);
		emitter->PreRender(m_scene->m_cameras[0]);
	}

	for(size_t index = 0; index < m_emitters.size(); ++index)
	{
		if(m_emitters[index]->IsReadyToDestroy() && m_emitters[index]->m_killWhenDone)
		{
			RemoveEmitter(m_emitters[index]);
			index--;
		}
	}

	CheckAndRemoveBullets();
	CheckAndRemoveEnemies();

	if(!IsDevConsoleOpen()) // If Dev console is closed handle game input
	{
		ProcessInput();
		ProcessMouseInput();
	}
	rend->SetRenderMode(m_renderMode);

}

//-----------------------------------------------------------------------------------------------
// Renders the debug stuff
//
void GameState_Playing::DebugRender()
{
	std::string debugMode = "Lit";
	switch (m_renderMode)
	{
	case 0: debugMode = "Lit"; break;
	case 1: debugMode = "Vertex Normals"; break;
	case 2: debugMode = "Tangents"; break;
	case 3: debugMode = "Bi tangents"; break;
	case 4: debugMode = "Normal Map"; break;
	case 5: debugMode = "Normal Map (World Normals)"; break;
	case 6: debugMode = "Surface Color"; break;
	case 7: debugMode = "ShadowMap"; break;
	default: debugMode = "Bad"; break;
	}
	DebugRenderLogf(0.f, "DebugMode [Y|U]: %s", debugMode.c_str());
	DebugRenderLogf(0.f, "Grid Enabled [O]: %s", std::to_string(m_renderGrid).c_str());

	if(m_renderGrid)
	{
		DebugRenderGrid(0.f, Vector3::RIGHT, Vector3::FORWARD, 10);
	}
}

//-----------------------------------------------------------------------------------------------
// Renders the game state
//
void GameState_Playing::Render() const
{
	m_renderPath->Render(m_scene);

	Renderer* rend = Renderer::GetInstance();

	rend->SetCamera(m_uiCamera);
	rend->ResetDefaultMaterial();
	rend->SetDefaultMaterial();
	rend->SetDefaultTexture();

	if(CheckVictory())
	{
		RenderWinUI();
		return; // Don't render the other UI stuff
	}
	if(m_playerTank)
	{

		RenderUI();
		if(m_playerTank->m_isDead)
		{
			RenderDeathUI();
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Renders the game UI
//
void GameState_Playing::RenderUI() const
{
	Renderer* rend = Renderer::GetInstance();
	BitmapFont* font = rend->CreateOrGetBitmapFont("SquirrelFixedFont");

	AABB2 uiBounds = GetUIBounds();
	rend->DrawAABB2(uiBounds, Rgba(200,200,200,100));

	AABB2 healthBounds = GetHealthUIBounds();
	int health = m_playerTank->m_health;
	std::string uiString = Stringf("Health:%d/100", health);
	rend->DrawTextInBox2D(healthBounds, uiString, 0.02f, TextAlignment::ALIGN_BOTTOM_LEFT, OVERRUN, font, Rgba::RED);

	uiString = Stringf("Enemies:%d", m_enemies.size());
	AABB2 enemyUIBounds = GetEnemyUIBounds();
	rend->DrawTextInBox2D(enemyUIBounds, uiString, 0.02f, TextAlignment::ALIGN_BOTTOM_LEFT, OVERRUN, font, Rgba::RED);

	uiString = Stringf("Bases:%d", m_bases.size());
	AABB2 baseUIBounds = GetEnemyBaseUIBounds();
	rend->DrawTextInBox2D(baseUIBounds, uiString, 0.02f, TextAlignment::ALIGN_BOTTOM_LEFT, OVERRUN, font, Rgba::RED);
}

//-----------------------------------------------------------------------------------------------
// Renders death UI
//
void GameState_Playing::RenderDeathUI() const
{
	Renderer* rend = Renderer::GetInstance();
	BitmapFont* font = rend->CreateOrGetBitmapFont("SquirrelFixedFont");
	
	rend->SetShader(Shader::CreateOrGetResource("Data/Shaders/additive.shader"));
	rend->SetDefaultTexture();
	rend->DrawAABB2(AABB2::ZERO_TO_ONE, Rgba(255,0,0,100));
	
	float respawnElapsed = RESPAWN_TIME - m_respawnTimer->GetElapsedTime();
	respawnElapsed = ClampFloat(respawnElapsed, 0.f, RESPAWN_TIME);
	std::string respawnString = Stringf("Wait for %0.3f seconds", respawnElapsed);

	rend->DrawTextInBox2D(AABB2(Vector2(0.5f, 0.5f), 1.f, 1.f), "Press Enter To Continue", 0.03f, TextAlignment::ALIGN_CENTER, OVERRUN, font);
	rend->DrawTextInBox2D(AABB2(Vector2(0.5f, 0.1f), 1.f, 1.f), respawnString, 0.03f, TextAlignment::ALIGN_CENTER, OVERRUN, font);
}

//-----------------------------------------------------------------------------------------------
// Renders win UI
//
void GameState_Playing::RenderWinUI() const
{
	Renderer* rend = Renderer::GetInstance();
	BitmapFont* font = rend->CreateOrGetBitmapFont("SquirrelFixedFont");

	rend->SetShader(Shader::CreateOrGetResource("Data/Shaders/additive.shader"));
	rend->SetDefaultTexture();
	rend->DrawAABB2(AABB2::ZERO_TO_ONE, Rgba(0,255,0,200));

	float respawnElapsed = RESPAWN_TIME - m_respawnTimer->GetElapsedTime();
	respawnElapsed = ClampFloat(respawnElapsed, 0.f, RESPAWN_TIME);
	std::string respawnString = Stringf("Wait for %0.3f seconds", respawnElapsed);

	rend->DrawTextInBox2D(AABB2(Vector2(0.5f, 0.5f), 1.f, 1.f), "Press Enter To Return to MainMenu", 0.03f, TextAlignment::ALIGN_CENTER, OVERRUN, font);
}

//-----------------------------------------------------------------------------------------------
// Checks for victory and triggers victory UI
//
bool GameState_Playing::CheckVictory() const
{
	return m_enemies.size() <= 0 && m_bases.size() <= 0;
}

//-----------------------------------------------------------------------------------------------
// Raycast to all gameobjects including terrain and return the first one to hit
//
RaycastHit GameState_Playing::Raycast(Ray3& ray, float maxDistance)
{
	RaycastHit terrainHit;
	
	// Raycast against terrain
	terrainHit = m_map->Raycast(ray, maxDistance);

	// Raycast against enemies and bases
	float stepSize = maxDistance / (float) MAX_RAYCAST_STEPS;
	RaycastHit enemyHit;
	for(int i = 0; i < MAX_RAYCAST_STEPS; ++i)
	{
		Vector3 rayPos = ray.Evaluate(i * stepSize);
		enemyHit = CheckPosAgainstEnemies(rayPos);
		if(!enemyHit.hit)
		{
			enemyHit = CheckPosAgainstBases(rayPos);
		}

		if(enemyHit.hit)
		{
			enemyHit.position = ray.Evaluate((i-1) * stepSize);
			break;
		}
	}

	if(terrainHit.hit && enemyHit.hit)
	{
		Vector3 terrainDisplacement = terrainHit.position - ray.start;
		Vector3 enemyDisplacement = enemyHit.position - ray.start;
		
		if(terrainDisplacement.GetLengthSquared() > enemyDisplacement.GetLengthSquared())
		{
			return enemyHit;
		}
		else
		{
			return terrainHit;
		}
	}
	else if(terrainHit.hit)
	{
		return terrainHit;
	}
	else
	{
		return enemyHit;
	}
}

//-----------------------------------------------------------------------------------------------
// Raycasts to entities
//
RaycastHit GameState_Playing::CheckPosAgainstEnemies(const Vector3& pos)
{
	for(EnemyTank* enemy : m_enemies)
	{
		if(enemy->IsPointInside(pos))
		{
			RaycastHit hitResult;
			hitResult.hit = true;
			hitResult.position = pos;

			return hitResult;
		}
	}
	return RaycastHit();
}

//-----------------------------------------------------------------------------------------------
// Raycasts to enemy base
//
RaycastHit GameState_Playing::CheckPosAgainstBases(const Vector3& pos)
{
	for(EnemySpawn* base : m_bases)
	{
		if(base->IsPointInside(pos))
		{
			RaycastHit hitResult;
			hitResult.hit = true;
			hitResult.position = pos;

			return hitResult;
		}
	}
	return RaycastHit();
}

//-----------------------------------------------------------------------------------------------
// Adds the tank to the scene
//
void GameState_Playing::AddTank(Tank* tank)
{
	m_scene->AddRenderable(tank->GetRenderable());
	m_gameObjects.push_back(tank);
}

//-----------------------------------------------------------------------------------------------
// Adds a bullet to the scene
//
void GameState_Playing::AddBullet(Bullet* bullet)
{
	m_scene->AddRenderable(bullet->GetRenderable());
	m_bullets.push_back(bullet);
}

//-----------------------------------------------------------------------------------------------
// Adds the enemy to the scene
//
void GameState_Playing::AddEnemy(EnemyTank* enemy)
{
	m_scene->AddRenderable(enemy->GetRenderable());
	m_enemies.push_back(enemy);
}

//-----------------------------------------------------------------------------------------------
// Adds the enemy spawner to the scene
//
void GameState_Playing::AddEnemySpawn(EnemySpawn* spawner)
{
	m_scene->AddRenderable(spawner->GetRenderable());
	m_bases.push_back(spawner);
}

//-----------------------------------------------------------------------------------------------
// Disables the player's renderable
//
void GameState_Playing::DisablePlayer()
{
	StartRespawnTimer();
	AudioSystem::GetInstance()->PlayOneOffGroup("enemy.hit");
	m_scene->RemoveRenderable(m_playerTank->GetRenderable());
	m_scene->RemoveRenderable(m_playerTank->GetTurretRenderable());
}

//-----------------------------------------------------------------------------------------------
// Respawns the player's renderable
//
void GameState_Playing::RespawnPlayer()
{
	m_scene->AddRenderable(m_playerTank->GetRenderable());
	m_scene->AddRenderable(m_playerTank->GetTurretRenderable());
	m_playerTank->m_isDead = false;
	m_playerTank->m_health = 100;
}

//-----------------------------------------------------------------------------------------------
// Checks and removes enemies based on their ready to destroy flag
//
void GameState_Playing::CheckAndRemoveEnemies()
{
	for(size_t enemyIndex = 0; enemyIndex < m_enemies.size(); ++enemyIndex)
	{
		if(m_enemies[enemyIndex]->IsReadyToDestroy())
		{
			DestroyEnemy(enemyIndex,m_enemies[enemyIndex]);
			enemyIndex--;
		}
	}

	for(size_t baseIndex = 0; baseIndex < m_bases.size(); ++baseIndex)
	{
		if(m_bases[baseIndex]->IsReadyToDestroy())
		{
			DestroyEnemyBase(baseIndex,m_bases[baseIndex]);
			baseIndex--;
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Checks and removes enemies based on their ready to destroy flag
//
void GameState_Playing::CheckAndRemoveBullets()
{
	for(size_t bulletIndex = 0; bulletIndex < m_bullets.size(); ++bulletIndex)
	{
		if(m_bullets[bulletIndex]->IsReadyToDestroy())
		{
			DestroyBullet(bulletIndex,m_bullets[bulletIndex]);
			bulletIndex--;
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Destroys the enemy and removes it from the list
//
void GameState_Playing::DestroyEnemy(size_t index, EnemyTank* enemy)
{
	m_scene->RemoveRenderable(enemy->GetRenderable());

	delete m_enemies[index];
	m_enemies[index] = m_enemies[m_enemies.size() - 1];
	m_enemies.pop_back();
}

//-----------------------------------------------------------------------------------------------
// Destroys the enemy base and removes it from the list
//
void GameState_Playing::DestroyEnemyBase(size_t index, EnemySpawn* spawner)
{
	m_scene->RemoveRenderable(spawner->GetRenderable());

	delete m_bases[index];
	m_bases[index] = m_bases[m_bases.size() - 1];
	m_bases.pop_back();
}

//-----------------------------------------------------------------------------------------------
// Destroys the bullet and its references
//
void GameState_Playing::DestroyBullet(size_t index, Bullet* bullet)
{
	m_scene->RemoveRenderable(bullet->GetRenderable());

	delete m_bullets[index];
	m_bullets[index] = m_bullets[m_bullets.size() - 1];
	m_bullets.pop_back();
}

//-----------------------------------------------------------------------------------------------
// Starts the respawn timer
//
void GameState_Playing::StartRespawnTimer()
{
	m_respawnTimer->Start();
}

//-----------------------------------------------------------------------------------------------
// Stops the respawn timer
//
void GameState_Playing::StopRespawnTimer()
{
	m_respawnTimer->Stop();
}

//-----------------------------------------------------------------------------------------------
// Kills all enemies
//
void GameState_Playing::KillEnemies()
{
	for(EnemyTank* enemy : m_enemies)
	{
		enemy->m_isReadyToDestroy = true;
	}
}

//-----------------------------------------------------------------------------------------------
// Kills all bases
//
void GameState_Playing::KillBases()
{
	for(EnemySpawn* base : m_bases)
	{
		base->m_isReadyToDestroy = true;
	}
}

//-----------------------------------------------------------------------------------------------
// Kills all enemies
//
bool GameState_Playing::KillAllCommand(Command& cmd)
{
	GameState_Playing* gState = (GameState_Playing*) g_curState;
	gState->KillBases();
	gState->KillEnemies();

	UNUSED(cmd);
	return true;
}
