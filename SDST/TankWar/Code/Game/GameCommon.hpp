#pragma once
#include "Engine/Math/IntVector2.hpp"

//-----------------------------------------------------------------------------------------------
// Forward declarations
class AudioSystem;
class GameState;

//-----------------------------------------------------------------------------------------------
// Extern declarations

extern	bool		g_isPaused;
extern	bool		g_isTimeSlow;

//-----------------------------------------------------------------------------------------------
// Constants
const		float MINIMUM_FPS_SUPPORTED = 30.f;
constexpr	float CLIENT_ASPECT = 1.77f;

//-----------------------------------------------------------------------------------------------
// Game Globals
extern GameState*	g_curState;

//-----------------------------------------------------------------------------------------------
// Constants
constexpr int MAX_RAYCAST_STEPS = 100;
const float	GRAVITY = 9.8f;
const float	RESPAWN_TIME = 5.f;

const float CAMERA_MOVE_SPEED = 10.f;
const float CAMERA_ROTATION_SPEED = 10.f;
const float	CAMERA_ZOOM_SPEED = 10.f;

const float	TANK_MOVE_SPEED = 10.f;
const float TANK_ROTATION_SPEED = 40.f;
const float TURRET_ROTATION_SPEED = 20.f;
const float	BULLET_MOVE_SPEED = 20.f;

//-----------------------------------------------------------------------------------------------
// Cardinal Constants
extern	const IntVector2 STEP_NORTH; //= IntVector2( 0,  1);
extern	const IntVector2 STEP_SOUTH; //= IntVector2( 0, -1);
extern	const IntVector2 STEP_EAST ; //= IntVector2( 1,  0);
extern	const IntVector2 STEP_WEST ; //= IntVector2(-1,  0);

//-----------------------------------------------------------------------------------------------
// Swarm weights
const	float	SWARM_SEEK_WEIGHT = 20.f;
const	float	SWARM_SEPARATE_WEIGHT = 30.f;
const	float	SWARM_COHESION_WEIGHT = 1.f;
const	float	SWARM_ALIGNMENT_WEIGHT = 10.f;

const	float	SWARM_MOVE_SPEED = 5.f;

const	float	SWARM_LOCAL_RADIUS = 30.f;
const	float	SWARM_SEPARATION_RADIUS = 3.f;
const	float	SWARM_COHESION_RADIUS = 6.f;

//-----------------------------------------------------------------------------------------------
// Spawner constants
const	float	ENEMY_SPAWN_RATE = 2.f;
const	int		ENEMY_TOTAL_COUNT = 10;


