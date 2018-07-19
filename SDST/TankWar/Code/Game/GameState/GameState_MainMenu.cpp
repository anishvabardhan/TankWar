#include "Game/GameState/GameState_MainMenu.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/Menu.hpp"
#include "Game/GameState/GameState_ReadyUp.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Core/Window.hpp"
//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
// Constructor
//
GameState_MainMenu::GameState_MainMenu()
	: GameState()
{
	Window* window = Window::GetInstance();
	m_mainMenu = new Menu();
	m_mainMenu->AddOption("PLAY", 0);
	m_mainMenu->AddOption("QUIT", 1);
	m_mainMenu->SetCenter(Vector2(window->m_width * 0.5f, window->m_height * 0.5f));
	m_music = AudioSystem::GetInstance()->PlayOneOffGroup("music.mainmenu");
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
GameState_MainMenu::~GameState_MainMenu()
{
	delete m_mainMenu;
	m_mainMenu = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Updates the state
//
void GameState_MainMenu::Update(float deltaSeconds)
{
	PROFILE_LOG_SCOPE_FUNCTION();

	if(!IsDevConsoleOpen())
	{
		ProcessInput();
		ProcessMouseInput();
	}
	
	UNUSED(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Renders the state
//
void GameState_MainMenu::Render() const
{
	Renderer* rend = Renderer::GetInstance();
	rend->ResetDefaultMaterial();
	m_mainMenu->Render();
}

//-----------------------------------------------------------------------------------------------
// Process input for the state
//
void GameState_MainMenu::ProcessInput()
{
	m_mainMenu->ProcessInput();
	if(m_mainMenu->GetFrameSelection() == 0)
	{
		AudioSystem::GetInstance()->StopSound(m_music);
		Game::GetInstance()->ChangeState( new GameState_ReadyUp() );
	}
	if(m_mainMenu->GetFrameSelection() == 1)
	{
		AudioSystem::GetInstance()->StopSound(m_music);
		App::GetInstance()->RequestQuit();
	}
}

//-----------------------------------------------------------------------------------------------
// Process mouse input for the state
//
void GameState_MainMenu::ProcessMouseInput()
{
}
