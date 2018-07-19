#pragma once
#include "Game/Tank.hpp"
#include "Engine/Math/Disc3.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class OrbitCamera;
class GameState;
class Game;
class StopWatch;
class GameState_Playing;
class Renderable;

//-----------------------------------------------------------------------------------------------
class PlayerTank : public Tank
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	PlayerTank();
	~PlayerTank();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
				Renderable* GetTurretRenderable() const { return m_turretRenderable; }
	
	//-----------------------------------------------------------------------------------------------
	// Methods
				void		Initialize();
	virtual		void		Update( float deltaSeconds ) override;
				void		UpdateTarget();
				void		OrientTowardTarget( float deltaSeconds );
	virtual		void		Render() const override;
				void		ProcessInput( float deltaSeconds );
				void		ProcessMouseInput( float deltaSeconds );
				void		FireBullet() const;
				void		TakeDamage( int damage );
				
	//-----------------------------------------------------------------------------------------------
	// Members
				Vector3				m_target;
				OrbitCamera*		m_camera;
				Game*				m_gameInstance;
				StopWatch*			m_fireInterval;
				Renderable*			m_turretRenderable;
				Transform*			m_turretTransform;
				float				m_cameraYaw = 0.f;
				float				m_cameraPitch = 0.f;
				float				m_cameraZoom = 5.f;
				GameState_Playing*	m_gameState;
				int					m_health = 100;
				bool				m_isDead = false;
				Disc3				m_sphereCollider;
};

