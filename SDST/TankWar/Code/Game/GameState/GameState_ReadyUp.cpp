#include "Game/GameState/GameState_ReadyUp.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/GameState/GameState_Playing.hpp"
#include "Game/Game.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
// Constructor
//
GameState_ReadyUp::GameState_ReadyUp()
	: GameState()
{

}

//-----------------------------------------------------------------------------------------------
// Destructor
//
GameState_ReadyUp::~GameState_ReadyUp()
{

}

//-----------------------------------------------------------------------------------------------
// Updates the state
//
void GameState_ReadyUp::Update(float deltaSeconds)
{
	if(!IsDevConsoleOpen())
	{
		ProcessInput();
		ProcessMouseInput();
	}
	UNUSED(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Render the state
//
void GameState_ReadyUp::Render() const
{
	Renderer* rend = Renderer::GetInstance();
	rend->SetCamera(nullptr);
	rend->SetOrtho(-1.f, 1.f, -1.f, 1.f, 0.f, 1.f);
	rend->ResetDefaultMaterial();
	rend->SetDefaultMaterial();
	rend->SetShader(Shader::CreateOrGetResource("Data/Shaders/text.shader"));
	rend->DrawTextInBox2D(AABB2(Vector2::ZERO, 1.f, 1.f), "Press Enter To Continue", 0.05f, TextAlignment::ALIGN_CENTER, OVERRUN, rend->CreateOrGetBitmapFont("SquirrelFixedFont"));
}

//-----------------------------------------------------------------------------------------------
// Process key input for the state
//
void GameState_ReadyUp::ProcessInput()
{
	InputSystem* input = InputSystem::GetInstance();
	if(input->WasKeyJustPressed(KEYCODE_ENTER))
	{
		Game::GetInstance()->ChangeState( new GameState_Playing() );
	}
}

//-----------------------------------------------------------------------------------------------
// Process mouse input for the state
//
void GameState_ReadyUp::ProcessMouseInput()
{
}
