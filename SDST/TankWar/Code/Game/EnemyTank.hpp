#pragma once
#include "Game/Tank.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Disc3.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class GameMap;
class GameState_Playing;

//-----------------------------------------------------------------------------------------------
class EnemyTank : public GameObject
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	EnemyTank();
	~EnemyTank(){}
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
			Vector3		GetWorldPos() const;
			bool		IsReadyToDestroy() const { return m_isReadyToDestroy; }
	
	//-----------------------------------------------------------------------------------------------
	// Methods
	virtual void		Update(float deltaSeconds) override;
	virtual void		Render() const override;
			Matrix44	ComputeAlignmentMatrix();

			bool		IsPointInside( const Vector3& pos );
			void		ResetForces();
			void		UpdateLocalFlock();
			void		UpdatePlayerSeekForce();
			void		UpdateSeparationForce();
			void		UpdateCohesiveForce();
			void		UpdateAlignmentForce();
			void		OrientToVelocity( float deltaSeconds );
			void		CheckResolvePlayerCollision();

	//-----------------------------------------------------------------------------------------------
	// Members
	GameMap*				m_map;
	GameState_Playing*		m_gameState;

	std::vector<EnemyTank*> m_localFlock;
	Vector3					m_velocity = Vector3::FORWARD;
	Vector3					m_seekForce;
	Vector3					m_separationForce;
	Vector3					m_cohesiveForce;
	Vector3					m_alignmentForce;
	Disc3					m_sphereCollider;
	bool					m_isReadyToDestroy = false;
};

