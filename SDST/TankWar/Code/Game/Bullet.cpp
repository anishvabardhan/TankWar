#include "Game/Bullet.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/GameCommon.hpp"
#include "Game/GameState/GameState_Playing.hpp"
#include "Game/GameMap.hpp"
#include "Game/EnemyTank.hpp"
#include "Game/EnemySpawn.hpp"

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Renderer/Lights/Light.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Audio/AudioSystem.hpp"
//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
// Constructor
//
Bullet::Bullet()
	: GameObject()
{
	Mesh* bulletMesh = CreateCube(Vector3::ZERO, Vector3(0.2f));
	Material* bulletMat = Renderer::GetInstance()->CreateOrGetMaterial("Data/Materials/bullet.mat");

	m_renderable->SetMesh(bulletMesh);
	m_renderable->SetMaterial(*bulletMat);

	m_gameState = (GameState_Playing*) g_curState;
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
Bullet::~Bullet()
{

}

//-----------------------------------------------------------------------------------------------
// Updates the bullet
//
void Bullet::Update(float deltaSeconds)
{
	Vector3 newPos = m_direction * deltaSeconds * BULLET_MOVE_SPEED;
	m_transform->Translate(newPos);

	if(CheckResolveTerrainCollision())
		return;
	if(CheckResolveBaseCollision())
		return;
	if(CheckResolveEnemyCollision())
		return;
}

//-----------------------------------------------------------------------------------------------
// Renders extra details on the bullet
//
void Bullet::Render() const
{

}

//-----------------------------------------------------------------------------------------------
// Checks for terrain collision and destroys on impact
//
bool Bullet::CheckResolveTerrainCollision()
{
	if(m_gameState->m_map->IsPointBelow(m_transform->GetWorldPosition()))
	{
		m_isReadyToDestroy = true;
	}

	return m_isReadyToDestroy;
}

//-----------------------------------------------------------------------------------------------
// Checks for enemy collision and destroys on impact
//
bool Bullet::CheckResolveEnemyCollision()
{
	Vector3 bulletPos = m_transform->GetWorldPosition();
	std::vector<EnemyTank*>& enemies = m_gameState->m_enemies;
	for(size_t index = 0; index < enemies.size(); ++index)
	{
		if(enemies[index]->IsPointInside(bulletPos))
		{
			m_isReadyToDestroy = true;
			enemies[index]->m_isReadyToDestroy = true;
			AudioSystem::GetInstance()->PlayOneOffGroup("enemy.die");
		}
	}

	return m_isReadyToDestroy;
}

//-----------------------------------------------------------------------------------------------
// Checks for enemy base collision and destroys on impact
//
bool Bullet::CheckResolveBaseCollision()
{
	Vector3 bulletPos = m_transform->GetWorldPosition();
	std::vector<EnemySpawn*>& bases = m_gameState->m_bases;
	for(size_t index = 0; index < bases.size(); ++index)
	{
		if(bases[index]->IsPointInside(bulletPos))
		{
			m_isReadyToDestroy = true;
			bases[index]->m_isReadyToDestroy = true;
			AudioSystem::GetInstance()->PlayOneOffGroup("enemy.die");
		}
	}

	return m_isReadyToDestroy;
}
