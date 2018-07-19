#pragma once
#include "Game/GameState/GameState.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Menu;

//-----------------------------------------------------------------------------------------------
class GameState_MainMenu : public GameState
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	GameState_MainMenu();
	~GameState_MainMenu();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	
	//-----------------------------------------------------------------------------------------------
	// Methods
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	virtual void ProcessInput() override;
	virtual void ProcessMouseInput() override;
	
	//-----------------------------------------------------------------------------------------------
	// Members
	Menu*	m_mainMenu;
	size_t	m_music;
};


