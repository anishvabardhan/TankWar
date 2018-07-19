#pragma once
#include "Game/GameObject.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Light;
class GameState_Playing;

//-----------------------------------------------------------------------------------------------
class Bullet : public GameObject
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	Bullet();
	~Bullet();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
				bool	IsReadyToDestroy() const { return m_isReadyToDestroy; }

	//-----------------------------------------------------------------------------------------------
	// Methods
	virtual		void	Update( float deltaSeconds ) override;
	virtual		void	Render() const override;
				bool	CheckResolveTerrainCollision();
				bool	CheckResolveEnemyCollision();
				bool	CheckResolveBaseCollision();

	//-----------------------------------------------------------------------------------------------
	// Members
	Vector3				m_direction = Vector3::FORWARD;
	GameState_Playing*	m_gameState;
	bool				m_isReadyToDestroy = false;
};

