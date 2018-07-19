#pragma once
#include "Game/GameState/GameState.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations


//-----------------------------------------------------------------------------------------------
class GameState_ReadyUp : public GameState
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	GameState_ReadyUp();
	~GameState_ReadyUp();
	
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
	

};

