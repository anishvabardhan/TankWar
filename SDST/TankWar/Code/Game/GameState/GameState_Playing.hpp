#pragma once
#include "Game/GameState/GameState.hpp"
#include <vector>

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Command;
class ForwardRenderPath;
class RenderScene;
class GameObject;
class GameMap;
class ParticleEmitter;
class PlayerTank;
class Tank;
class Renderable;
class Transform;
class EnemySpawn;
class EnemyTank;
class Ray3;
struct RaycastHit3D;
class Bullet;
class AABB2;
class Camera;
class StopWatch;
class Vector3;

//-----------------------------------------------------------------------------------------------
class GameState_Playing : public GameState
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	GameState_Playing();
	~GameState_Playing();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
			AABB2			GetUIBounds() const;
			AABB2			GetHealthUIBounds() const;
			AABB2			GetEnemyUIBounds() const;
			AABB2			GetEnemyBaseUIBounds() const;

	//-----------------------------------------------------------------------------------------------
	// Methods
			void			CreateScene();
			void			AddEmitter( ParticleEmitter* emitter );
			void			RemoveEmitter( ParticleEmitter* emitter );
	virtual void			ProcessInput() override;
	virtual void			ProcessMouseInput() override;
	virtual void			Update( float deltaSeconds ) override;
			void			DebugRender();
	virtual void			Render() const override;
			void			RenderUI() const;
			void			RenderDeathUI() const;
			void			RenderWinUI() const;
			bool			CheckVictory() const;

	//-----------------------------------------------------------------------------------------------
	// Game Methods
			RaycastHit3D	Raycast( Ray3& ray, float maxDistance );
			RaycastHit3D	CheckPosAgainstEnemies( const Vector3& position );
			RaycastHit3D	CheckPosAgainstBases( const Vector3& pos );
			void			AddTank( Tank*	tank );
			void			AddBullet ( Bullet* bullet );
			void			AddEnemy( EnemyTank* enemy );
			void			AddEnemySpawn( EnemySpawn* spawner );
			void			DisablePlayer();
			void			RespawnPlayer();
			void			CheckAndRemoveEnemies();
			void			CheckAndRemoveBullets();
			void			DestroyEnemy( size_t index, EnemyTank* enemy );
			void			DestroyEnemyBase( size_t index, EnemySpawn* spawner );
			void			DestroyBullet( size_t index, Bullet* bullet );
			void			StartRespawnTimer();
			void			StopRespawnTimer();
			void			KillEnemies();
			void			KillBases();
	//-----------------------------------------------------------------------------------------------
	// Command Callbacks
	static	bool			KillAllCommand( Command& cmd );

	//-----------------------------------------------------------------------------------------------
	// Members
	int								m_renderMode = 0;
	ForwardRenderPath*				m_renderPath;
	bool							m_renderGrid = false;
	GameMap*						m_map;
	PlayerTank*						m_playerTank;
	std::vector<EnemyTank*>			m_enemies;
	std::vector<EnemySpawn*>		m_bases;
	std::vector<Bullet*>			m_bullets;
	Renderable*						m_waterRenderable;
	Transform*						m_waterTransform;
	size_t							m_musicPlaybackID;

	Camera*							m_uiCamera;
	StopWatch*						m_respawnTimer;
};

