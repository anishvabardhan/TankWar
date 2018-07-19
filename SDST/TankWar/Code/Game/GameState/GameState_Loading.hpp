#pragma once
#include "Game/GameState/GameState.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations


//-----------------------------------------------------------------------------------------------
class GameState_Loading : public GameState
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	GameState_Loading();
	~GameState_Loading(){}
	
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
	bool			m_isFirstFrame = true;
};

