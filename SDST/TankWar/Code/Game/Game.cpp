#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameState/GameState.hpp"
#include "Game/GameState/GameState_Playing.hpp"
#include "Game/GameState/GameState_Loading.hpp"

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Console/CommandDefinition.hpp"
#include "Engine/Console/Command.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Renderer/DebugRenderer.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/StopWatch.hpp"
#include "Engine/Profiler/Profiler.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Static Globals
static Game* g_theGame = nullptr;

//-----------------------------------------------------------------------------------------------
// Constructor
//
Game::Game()
{
	Initialize();

	GameState_Loading* gState = new GameState_Loading();
	PushState( gState );
	g_curState = gState;
	//gState->m_scene->AddCamera(m_gameCamera);
	InputSystem::GetInstance()->ShowCursor(false);

	LoadAudioFiles();
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
Game::~Game()
{
	delete m_gameCamera;
	m_gameCamera = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Returns true if the game is paused
//
bool Game::IsPaused()
{
	return g_isPaused;
}

//-----------------------------------------------------------------------------------------------
// Returns true if the time is slowed
//
bool Game::IsTimeSlow()
{
	return g_isTimeSlow;
}

//-----------------------------------------------------------------------------------------------
// Initializes the game elements
//
void Game::Initialize()
{
	g_isPaused = false;
	g_isTimeSlow = false;
	m_gameClock = new Clock(Clock::GetMasterClock()); // Creates game clock as a child of master clock
	
	// Initialize game_config blackboard 
	DebuggerPrintf("\nReading game configuration from Data/gameconfig.xml\n");
	tinyxml2::XMLDocument gameConfigDoc;
	gameConfigDoc.LoadFile("Data/gameconfig.xml");
	g_gameConfigBlackboard.PopulateFromXmlElementAttributes( *gameConfigDoc.FirstChildElement() );
	DebuggerPrintf("\nGame config loaded successfully.\n");
}

//-----------------------------------------------------------------------------------------------
// Loads audio files and stores the sound ids
//
void Game::LoadAudioFiles()
{
	AudioSystem* audio = AudioSystem::GetInstance();
	UNUSED(audio);
}

//-----------------------------------------------------------------------------------------------
// Pushes a state on the state stack
//
void Game::PushState(GameState* state)
{
	m_stateStack.push(state);
}

//-----------------------------------------------------------------------------------------------
// Pops a state off the state stack
//
void Game::PopState()
{
	m_stateStack.pop();
}

//-----------------------------------------------------------------------------------------------
// Transitions through states
//
void Game::ChangeState(GameState* state)
{
	PopState();
	m_stateStack.push(state);
}

//-----------------------------------------------------------------------------------------------
// Updates the top of the state stack
//
void Game::ProcessStates(float deltaSeconds)
{
	PROFILE_LOG_SCOPE_FUNCTION();

	GameState* gState = m_stateStack.top();
	gState->Update(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Renders the state on the top of the stack
//
void Game::RenderTopState() const
{
	GameState* gState = m_stateStack.top();
	gState->Render();
}

//-----------------------------------------------------------------------------------------------
// Updates the Gameplay state
//
void Game::Update()
{
	PROFILE_LOG_SCOPE_FUNCTION();

	float deltaSeconds = (float) m_gameClock->GetDeltaSeconds();
	deltaSeconds = ClampFloat(deltaSeconds, 0.f, 1.f / MINIMUM_FPS_SUPPORTED);

	if(g_isTimeSlow)
		m_gameClock->SetTimeScale(0.1f);
	else
		m_gameClock->ResetTimeScale();

	if(g_isPaused)
		m_gameClock->PauseClock();
	else
		m_gameClock->ResumeClock();

	ProcessStates(deltaSeconds); // Calls update on the stack top
}

//-----------------------------------------------------------------------------------------------
// Renders the Game
//
void Game::Render() const
{
	PROFILE_LOG_SCOPE_FUNCTION();

	Renderer::GetInstance()->SetCamera(Game::GetInstance()->m_gameCamera);
	Renderer::GetInstance()->ClearDepth();
	Renderer::GetInstance()->ClearScreen(Rgba::BLACK);
	RenderTopState();
}

//-----------------------------------------------------------------------------------------------
// Creates the game instance
//
Game* Game::CreateInstance()
{
	if(g_theGame == nullptr)
	{
		g_theGame = new Game();
	}

	return g_theGame;
}

//-----------------------------------------------------------------------------------------------
// Returns the game instance
//
Game* Game::GetInstance()
{
	return g_theGame;
}

//-----------------------------------------------------------------------------------------------
// Destroys the game instance
//
void Game::DestroyInstance()
{
	if(g_theGame)
	{
		delete g_theGame;
		g_theGame = nullptr;
	}
}
