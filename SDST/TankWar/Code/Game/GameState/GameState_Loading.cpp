#include "Game/GameState/GameState_Loading.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameState/GameState_MainMenu.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
// Constructor
//
GameState_Loading::GameState_Loading()
	: GameState()
{
}

//-----------------------------------------------------------------------------------------------
// Updates the state
//
void GameState_Loading::Update(float deltaSeconds)
{
	if(m_isFirstFrame)
	{
		m_isFirstFrame = false;
		return;
	}

	Renderer::GetInstance()->CreateOrGetCubeMap("Data/Images/skybox.jpg");
	AudioSystem::LoadAudioGroupsFromXML("Data/Audio/sfx.xml");

	Game::GetInstance()->ChangeState( new GameState_MainMenu() );

	UNUSED(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Renders the loading state
//
void GameState_Loading::Render() const
{
	Renderer* rend = Renderer::GetInstance();
	rend->SetCamera(nullptr);
	rend->SetOrtho(-1.f, 1.f, -1.f, 1.f, 0.f, 1.f);
	rend->ResetDefaultMaterial();
	rend->SetDefaultMaterial();
	rend->SetTexture(rend->CreateOrGetTexture("Data/Images/loading.gif"));
	rend->DrawAABB2(AABB2(Vector2::ZERO, 1.f, 1.f));
}

//-----------------------------------------------------------------------------------------------
// Process input in loading state
//
void GameState_Loading::ProcessInput()
{
}

//-----------------------------------------------------------------------------------------------
// Process mouse input
//
void GameState_Loading::ProcessMouseInput()
{
}
