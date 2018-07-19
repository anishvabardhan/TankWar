#pragma once
#include "Game/GameObject.hpp"
#include "Engine/Math/AABB3.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class GameState_Playing;
class GameMap;
class StopWatch;

//-----------------------------------------------------------------------------------------------
class EnemySpawn : public GameObject
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	EnemySpawn();
	~EnemySpawn(){}
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	void	SetXZPosition( const Vector2& pos );
	bool	IsReadyToDestroy() const { return m_isReadyToDestroy; }
	
	//-----------------------------------------------------------------------------------------------
	// Methods
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
			void SpawnEnemy();
			bool IsPointInside( const Vector3& pos ) const;
			void TakeDamage( int damage );	
	//-----------------------------------------------------------------------------------------------
	// Members
	GameState_Playing*	m_gameState;
	GameMap*			m_map;
	StopWatch*			m_spawnInterval;	
	int					m_spawnCounter = 0;
	AABB3				m_bounds;
	bool				m_isReadyToDestroy = false;
	int					m_health = 100;
};

