#pragma once
#include "Engine/Async/ThreadSafeQueue.hpp"
#include <string>
#include "Engine/Async/ThreadSafeVector.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Command;

//-----------------------------------------------------------------------------------------------
// Macros
constexpr char* DEFAULT_LOG_TAG = "log";
constexpr char* DEFAULT_WARNING_TAG = "warning";
constexpr char* DEFAULT_ERROR_TAG = "error";

//-----------------------------------------------------------------------------------------------
struct LogMessage
{
	std::string tag;
	std::string text;
};

typedef void (*LogCb)( const LogMessage& msg, void* userData );

//-----------------------------------------------------------------------------------------------
struct LogHookDef
{
	LogCb callback;
	void* userData;
};

//-----------------------------------------------------------------------------------------------
class Logger
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	Logger();
	~Logger();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
			bool IsRunning() const { return m_isRunning; }

	//-----------------------------------------------------------------------------------------------
	// Methods
			void Start() { m_isRunning = true; }
			void Stop() { m_isRunning = false; }
			void Flush();
			void AddHook( LogHookDef* hook );
			void AddHook( LogCb cb, void* userData );
			void RemoveHook( LogHookDef* hook );
			void RemoveHook( LogCb cb, void* userData );
			void AddLogMessage( LogMessage* msg );
			void AddLogMessage( const std::string& tag, const std::string& text );
			void EnableTag( const char* tag );
			void DisableTag( const char* tag );
			void EnableAll();
			void DisableAll();

	//-----------------------------------------------------------------------------------------------
	// Testing methods
			void Test(const char* fileName, int threadCount );

	//-----------------------------------------------------------------------------------------------
	// Static methods
	static	Logger*	CreateInstance();
	static	Logger*	GetInstance();
	static	void	DestroyInstance();
	static	void	LogThreadWorker( void* userData = nullptr );
	static	void	LogTestWorker( void* userData = nullptr );
	static	void	WriteLogToFile( const LogMessage& msg, void* filePtr );
	
	//-----------------------------------------------------------------------------------------------
	// Command callbacks
	static	bool	FlushTestCommand( Command& cmd );
	static	bool	LogTestCommand( Command& cmd );
	static	bool	LogEnableAllCommand( Command& cmd );
	static	bool	LogDisableAllCommand( Command& cmd );
	static	bool	LogEnableTagCommand( Command& cmd );
	static	bool	LogHideTagCommand( Command& cmd );

	//-----------------------------------------------------------------------------------------------
	// Members
	bool							m_isRunning = true;
	void*							m_thread;
	ThreadSafeQueue<LogMessage*>	m_logQueue;
	ThreadSafeVector<LogHookDef*>	m_hooks;
	ThreadSafeVector<std::string>	m_filters;
	bool							m_areFiltersWhiteList = false;
};

//-----------------------------------------------------------------------------------------------
// Standalone functions

// System Functions
void LogSystemStartup( const char* fileName = DEFAULT_LOG_TAG ); // Do not include extension in filename
void LogSystemShutdown();

// Logger functions
void LogTaggedPrintv( const char* tag, const char* format, va_list args );
void LogFlush();

// Logger Helpers
void LogTaggedPrintf( const char* tag, const char* format, ... );
void LogPrintf( const char* format, ... ); // Defaults to log tag 
void LogWarningf( const char* format, ... ); // Defaults to warning tag
void LogErrorf( const char* format, ... ); // Defaults to error tag. Flush and then assert at the end

// Filtering functions
void LogShowAll();
void LogHideAll();
void LogShowTag( const char* tag );
void LogHideTag( const char* tag );

// Hooking
void LogHook( LogCb cb, void* userData = nullptr );
void LogUnhook( LogCb cb, void* userData = nullptr );


