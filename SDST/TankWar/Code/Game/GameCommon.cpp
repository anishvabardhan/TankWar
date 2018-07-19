#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
bool		 g_isPaused = false;
bool		 g_isTimeSlow = false;

//-----------------------------------------------------------------------------------------------
const	float ORTHO_NEAR = 0.f;
const	float ORTHO_FAR = 1.f;

//-----------------------------------------------------------------------------------------------
GameState*	g_curState = nullptr;

//-----------------------------------------------------------------------------------------------
// Cardinal Constants
const IntVector2 STEP_NORTH = IntVector2( 0,  1);
const IntVector2 STEP_SOUTH = IntVector2( 0, -1);
const IntVector2 STEP_EAST  = IntVector2( 1,  0);
const IntVector2 STEP_WEST  = IntVector2(-1,  0);


