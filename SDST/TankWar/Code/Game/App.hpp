#pragma once

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Command;

//-----------------------------------------------------------------------------------------------
class App
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	App();
	~App();

	//-----------------------------------------------------------------------------------------------
	// Methods
	void Update();
	void Render();
	void BeginFrame();
	void EndFrame();
	void RunFrame();
	void RequestQuit();
	void HandleKeyboardInput();

	//-----------------------------------------------------------------------------------------------
	// Static methods
	static App* CreateInstance();
	static App* GetInstance();
	static void DestroyInstance();
	static bool IsQuitting();
	static void ThreadTestWork( void* );	

	//-----------------------------------------------------------------------------------------------
	// Command callbacks
	static bool ThreadTestCommand( Command& cmd );
	static bool	ThreadTestNoThreadCommand( Command& cmd );

	//-----------------------------------------------------------------------------------------------
	// Member Variables
	bool m_isQuitting = false;
};


