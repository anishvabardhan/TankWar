#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Logger/Logger.hpp"

//-----------------------------------------------------------------------------------------------
Blackboard	g_gameConfigBlackboard;

//-----------------------------------------------------------------------------------------------
// Starts up the engine systems
//
void EngineStartup()
{
	LogSystemStartup();
	ClockSystemStartup();
	RenderingSystemStartup();
	DebugRendererStartup();
	InputSystemStartup();
	AudioSystemStartup();
	ConsoleStartup();
	ProfilerStartup();
}

//-----------------------------------------------------------------------------------------------
// Shuts down the engine systems
//
void EngineShutdown()
{
	ConsoleShutdown();
	AudioSystemShutdown();
	InputSystemShutdown();
	DebugRendererShutdown();
	RenderingSystemShutdown();
	ProfilerShutdown();
	LogSystemShutdown();
}

