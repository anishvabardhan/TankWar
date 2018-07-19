#include "Game/App.hpp"

//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Console/Command.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/DebugRenderer.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Async/Thread.hpp"
#include "Engine/Console/CommandDefinition.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Static globals
static App* g_theApp = nullptr;

//-----------------------------------------------------------------------------------------------
// Constructor
//
App::App()
{
	// Initialize the engine
	EngineStartup();

	// Command registrations
	COMMAND("threadtest_threaded", ThreadTestCommand, "Tests the threading system");
	COMMAND("threadtest_nothread", ThreadTestNoThreadCommand, "Tests the non threaded system");

	// Initialize the game
	Game::CreateInstance();
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
App::~App()
{
	// Destroy the game first
	Game::DestroyInstance();

	// Shutdown the engine
	EngineShutdown();
}

//-----------------------------------------------------------------------------------------------
// Updates the App state
void App::Update()
{
	PROFILE_LOG_SCOPE_FUNCTION();

	float deltaSeconds = (float) Clock::GetMasterDeltaSeconds();
	ClampFloat(deltaSeconds, 0.f, 1.f/MINIMUM_FPS_SUPPORTED);

	// Input handlers
	HandleKeyboardInput();

	// Updates the game logic with the calculated deltaSeconds
	Game::GetInstance()->Update();

	// Updates the engine systems
	Profiler::GetInstance()->Update(deltaSeconds);
	DebugRenderer::GetInstance()->Update(deltaSeconds);
	DevConsole::GetInstance()->Update(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Renders the App 
//
void App::Render()
{
	Game* gameInstance = Game::GetInstance();
	gameInstance->Render();


	Profiler::GetInstance()->Render();

	DebugRenderer::GetInstance()->Render();
	if(IsDevConsoleOpen())
	{
		DevConsole::GetInstance()->Render();
	}
}

//-----------------------------------------------------------------------------------------------
// Starts the begin frame chain on the engine components and sets up screen
//
void App::BeginFrame()
{
	Clock::GetMasterClock()->BeginFrame(); // Ticks the master clock
	InputSystem::GetInstance()->BeginFrame();
	Renderer::GetInstance()->BeginFrame();
	AudioSystem::GetInstance()->BeginFrame();
}

//-----------------------------------------------------------------------------------------------
// Starts the end frame chain on the engine components
//
void App::EndFrame()
{
	AudioSystem::GetInstance()->EndFrame();
	Renderer::GetInstance()->EndFrame();
	InputSystem::GetInstance()->EndFrame();
	Clock::GetMasterClock()->EndFrame();
	Sleep(1); // For CPU Usage 
}

//-----------------------------------------------------------------------------------------------
// Starts the update chain on the game components and calculates delta time 
//
void App::RunFrame()
{
	Profiler::MarkFrame();
	BeginFrame();
	Update();
	Render();
	EndFrame();
}

//-----------------------------------------------------------------------------------------------
// Handles keyboard inputs
//
void App::HandleKeyboardInput()
{
	InputSystem* input = InputSystem::GetInstance();
	if(input->WasKeyJustPressed(KEYCODE_TILDE))
	{
		if(!IsDevConsoleOpen())
		{
			DevConsole::GetInstance()->Open();
		}

		else
		{
			DevConsole::GetInstance()->Close();
		}
	}

	if(input->WasKeyJustPressed(VK_DELETE))
	{
		Command::CommandRun("screenshot");
	}
}

//-----------------------------------------------------------------------------------------------
// Creates an app instance
//
App* App::CreateInstance()
{
	if(g_theApp == nullptr)
	{
		g_theApp = new App();
	}

	return g_theApp;
}

//-----------------------------------------------------------------------------------------------
// Returns the app instance
//
App* App::GetInstance()
{
	return g_theApp;
}

//-----------------------------------------------------------------------------------------------
// destroys the app instance
//
void App::DestroyInstance()
{
	if(g_theApp)
	{
		delete g_theApp;
		g_theApp = nullptr;
	}
}

//-----------------------------------------------------------------------------------------------
// Returns true if the app is quitting
//
bool App::IsQuitting()
{
	return g_theApp->m_isQuitting;
}

//-----------------------------------------------------------------------------------------------
// Utility to test if threads work
//
void App::ThreadTestWork(void*)
{
	FILE *fp = nullptr;
	fopen_s( &fp, "garbage.dat", "w" );

	fseek(fp, 0L, SEEK_SET);
	for(size_t i = 0; i < 12000000; ++i)
	{
		char test = (char) i;
		fwrite( (void*) &test, sizeof(int), 1, fp);
	}

	fclose(fp);
	DebuggerPrintf("Thread testing completed successfully.\n");
	ConsolePrintf("Thread testing completed successfully.\n");
}

//-----------------------------------------------------------------------------------------------
// Console command to test if threading works
//
bool App::ThreadTestCommand(Command& cmd)
{
	ThreadCreateAndDetach(ThreadTestWork, nullptr);

	UNUSED(cmd);
	return true;
}

//-----------------------------------------------------------------------------------------------
// Console command to test threading : This one is non-threaded
//
bool App::ThreadTestNoThreadCommand(Command& cmd)
{
	ThreadTestWork(nullptr);

	UNUSED(cmd);
	return true;
}

//-----------------------------------------------------------------------------------------------
// Quits the App
//
void App::RequestQuit()
{
	m_isQuitting = true;
}
