#include "Game/EnemySpawn.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/GameCommon.hpp"
#include "Game/GameMap.hpp"
#include "Game/GameState/GameState_Playing.hpp"
#include "Game/EnemyTank.hpp"

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Renderer/Mesh/MeshUtils.hpp"
#include "Engine/Renderer/Mesh/Mesh.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/StopWatch.hpp"

//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
// Constructor
//
EnemySpawn::EnemySpawn()
	: GameObject()
{
	Vector3 scale(1.f, 5.f, 1.f);
	Mesh* spawnMesh = CreateCube(Vector3::ZERO, scale);
	m_renderable->SetMesh(spawnMesh);

	Material* spawnMat = Renderer::GetInstance()->CreateOrGetMaterial("Data/Materials/spawn.mat");
	m_renderable->SetMaterial(*spawnMat);

	m_spawnInterval = new StopWatch();
	m_spawnInterval->SetTimer(ENEMY_SPAWN_RATE);

	scale *= 0.5f; // half extents
	m_bounds.mins = Vector3::ZERO - scale ;
	m_bounds.maxs = Vector3::ZERO + scale;
}

//-----------------------------------------------------------------------------------------------
// Gets the actual position from the map and sets it to the spawner transform
//
void EnemySpawn::SetXZPosition(const Vector2& pos)
{
	Vector3 actualPos = m_map->GetPositionForXZ(pos);
	m_transform->SetPosition(actualPos);
	m_bounds.SetCenter(actualPos);
}

//-----------------------------------------------------------------------------------------------
// Updates the spawner
//
void EnemySpawn::Update(float deltaSeconds)
{

	if(!m_gameState)
	{
		m_gameState = (GameState_Playing*) g_curState;
		return;
	}

	if(m_spawnInterval->Decrement() && m_spawnCounter < ENEMY_TOTAL_COUNT)
	{
		SpawnEnemy();
		SpawnEnemy();
	}

	(void) deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
// Renders extra stuff on the spawner
//
void EnemySpawn::Render() const
{
}

//-----------------------------------------------------------------------------------------------
// Spawns an enemy and adds it to the map
//
void EnemySpawn::SpawnEnemy()
{
	EnemyTank* enemy = new EnemyTank();
	enemy->SetPosition(m_transform->GetWorldPosition() + GetRandomPointOnSphere());
	enemy->m_map = m_map;
	
	m_gameState->AddEnemy(enemy);
	m_spawnCounter++;
}

//-----------------------------------------------------------------------------------------------
// Returns true if the point is inside
//
bool EnemySpawn::IsPointInside(const Vector3& pos) const
{
	return m_bounds.IsPointInside(pos);
}

//-----------------------------------------------------------------------------------------------
// Damages the enemy spawn
//
void EnemySpawn::TakeDamage(int damage)
{
	m_health -= damage;
	
	if(m_health <= 0)
	{
		m_isReadyToDestroy = true;
	}
}
