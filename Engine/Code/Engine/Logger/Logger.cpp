#include "Engine/Logger/Logger.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Async/Thread.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/File/File.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Console/CommandDefinition.hpp"
#include "Engine/Console/Command.hpp"
#include "Engine/Core/StringTokenizer.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Core/Platform/Win32.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Standard Includes
#include <stdarg.h>
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Static globals
static Logger*	s_logger = nullptr;
static File*	s_timeStampedLog = nullptr;
static File*	s_logFile = nullptr;

//-----------------------------------------------------------------------------------------------
// Starts up the logger system and timestamps the fileName
//
void LogSystemStartup(const char* fileName /*= DEFAULT_LOG_TAG */)
{
	Logger::CreateInstance();
	s_logger->m_thread = ThreadCreate("Logger Thread", s_logger->LogThreadWorker, nullptr);

	std::string logFileName = "Log/";
	logFileName += fileName;
	
	std::string defaultLogFile = logFileName + ".txt";

	// Creates the directory if it's not available
	if(!CreateDirectoryA("Log/", NULL)) 
	{
	}

	s_logFile = new File();
	int errorCode2 = s_logFile->Open(defaultLogFile.c_str(), FILE_WRITE); // Default log file

	//YYYYMMDD_HHMMSS
	std::string timeStamp = Time::GetSysTimeStamp();
	logFileName = logFileName + "_" + timeStamp + ".txt"; // Append the time stamp onto the fileName

	s_timeStampedLog = new File();
	int errorCode = s_timeStampedLog->Open(logFileName.c_str(), FILE_WRITE);

	
	if(errorCode != 0 && errorCode2 != 0) // fopen error codes
	{
		GUARANTEE_OR_DIE(false, "Couldn't create log file");
		delete s_timeStampedLog;
		s_timeStampedLog = nullptr;

		delete s_logFile;
		s_logFile = nullptr;
	}

	LogHook(Logger::WriteLogToFile, s_timeStampedLog);
	LogHook(PrintToIDE, nullptr);
}

//-----------------------------------------------------------------------------------------------
// Shutsdown the logger system 
//
void LogSystemShutdown()
{
	s_logger->Stop();
	ThreadJoin((ThreadHandle) s_logger->m_thread);
	
	s_logger->m_thread = nullptr;
	Logger::DestroyInstance();

	if(s_timeStampedLog != nullptr)
	{
		s_timeStampedLog->Close();
		delete s_timeStampedLog;
		s_timeStampedLog = nullptr;
	}
}

//-----------------------------------------------------------------------------------------------
// Constructor
//
Logger::Logger()
{
	COMMAND("logflushtest", FlushTestCommand, "Tests the log flush utility");
	COMMAND("logtest", LogTestCommand, "Threaded logger stress test");
	COMMAND("logenableall", LogEnableAllCommand, "Enables all tags for logging");
	COMMAND("logdisableall", LogDisableAllCommand,"Disables all tags for logging");
	COMMAND("logshowtag", LogEnableTagCommand, "Enables a single tag for logging");
	COMMAND("loghidetag", LogHideTagCommand, "Hides a single tag during logging");
	DisableTag("debug");

}

//-----------------------------------------------------------------------------------------------
// Destructor
//
Logger::~Logger()
{
	Flush();
}

//-----------------------------------------------------------------------------------------------
// Writes the message to the log file (Thread Safe)
//
void Logger::WriteLogToFile(const LogMessage& msg, void* filePtr)
{
	File* fp = (File*) filePtr;
	fp->Printf("%s: %s", msg.tag.c_str(), msg.text.c_str());
}

//-----------------------------------------------------------------------------------------------
// Command callback to test log flush utility
//
bool Logger::FlushTestCommand(Command& cmd)
{
	LogPrintf("LogFlushTest");
	LogFlush();

	UNUSED(cmd);
	return true;
}

//-----------------------------------------------------------------------------------------------
// Console command to test the logger
//
bool Logger::LogTestCommand(Command& cmd)
{

	std::string fileName = cmd.GetNextString();

	int threadCount;
	if(!cmd.GetNextInt(threadCount))
	{
		threadCount = 1;
	}

	s_logger->Test(fileName.c_str(), threadCount);

	return true;
}

//-----------------------------------------------------------------------------------------------
// Console command to enable all tags
//
bool Logger::LogEnableAllCommand(Command& cmd)
{
	UNUSED(cmd);
	LogShowAll();

	return true;
}

//-----------------------------------------------------------------------------------------------
// Console command to disable all tags 
//
bool Logger::LogDisableAllCommand(Command& cmd)
{
	UNUSED(cmd);
	LogHideAll();

	return true;
}

//-----------------------------------------------------------------------------------------------
// Console command to enable a single tag
//
bool Logger::LogEnableTagCommand(Command& cmd)
{
	std::string tag = cmd.GetNextString();
	LogShowTag(tag.c_str());

	return true;
}

//-----------------------------------------------------------------------------------------------
// Console command to hide a single tag
//
bool Logger::LogHideTagCommand(Command& cmd)
{
	std::string tag = cmd.GetNextString();
	LogHideTag(tag.c_str());

	return true;
}

//-----------------------------------------------------------------------------------------------
// Will ensure that before returning, all currently in flight log messages are complete, 
// and the file IO operations have been flushed
//
void Logger::Flush()
{
	LogMessage* msg;
	while(m_logQueue.Dequeue(&msg))
	{
		if((m_filters.Find(msg->tag) != UINT32_MAX) ^ m_areFiltersWhiteList)
		{
			delete msg; // Cleanup
			continue; // Filter found 
		}

		for(size_t index = 0; index < m_hooks.Size(); ++index)
		{
			m_hooks[index]->callback(*msg, m_hooks[index]->userData);
		}

		delete msg; // Cleanup
	}
}

//-----------------------------------------------------------------------------------------------
// Adds a hook to the list of hooks 
//
void Logger::AddHook(LogHookDef* hook)
{
	m_hooks.PushBack(hook);
}

//-----------------------------------------------------------------------------------------------
// Adds a hook to the list of hooks
//
void Logger::AddHook(LogCb cb, void* userData)
{
	LogHookDef* hook = new LogHookDef();
	hook->callback = cb;
	hook->userData = userData;

	AddHook(hook);
}

//-----------------------------------------------------------------------------------------------
// Removes a hook from the list of hooks
//
void Logger::RemoveHook(LogHookDef* hook)
{
	RemoveHook(hook->callback, hook->userData);
}

//-----------------------------------------------------------------------------------------------
// Removes the hook from the list of hooks that is identified by the callback and userdata
//
void Logger::RemoveHook(LogCb cb, void* userData)
{
	for(size_t index = 0; index < m_hooks.Size(); ++index)
	{
		if(m_hooks[index]->callback == cb && m_hooks[index]->userData == userData)
		{
			m_hooks.Erase(index);
			break;
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Adds a log message to the queue
//
void Logger::AddLogMessage(const std::string& tag, const std::string& text)
{
	LogMessage* msg = new LogMessage();
	msg->tag = tag;
	msg->text = text;

	AddLogMessage(msg);
}

//-----------------------------------------------------------------------------------------------
// Adds a log message to the queue
//
void Logger::AddLogMessage(LogMessage* msg)
{
	m_logQueue.Enqueue(msg);
}

//-----------------------------------------------------------------------------------------------
// If blacklist, tag is removed. If whitelist, tag is added
//
void Logger::EnableTag(const char* tag)
{
	if(m_areFiltersWhiteList == false)
	{
		m_filters.Erase(tag);
	}
	else
	{
		m_filters.PushBack(tag);
	}
}

//-----------------------------------------------------------------------------------------------
// If whitelist, tag is added. If blacklist, tag is removed
//
void Logger::DisableTag(const char* tag)
{
	if(m_areFiltersWhiteList)
	{
		m_filters.Erase(tag);
	}
	else
	{
		m_filters.PushBack(tag);
	}
}

//-----------------------------------------------------------------------------------------------
// Switches the filters to blacklist
//
void Logger::EnableAll()
{
	m_filters.Clear();
	m_areFiltersWhiteList = false;
}

//-----------------------------------------------------------------------------------------------
// Switches the filters to whitelist
//
void Logger::DisableAll()
{
	m_filters.Clear();
	m_areFiltersWhiteList = true;
}

//-----------------------------------------------------------------------------------------------
// Tests logger by spinning up threads that will read a file and log it
//
void Logger::Test(const char* fileName, int threadCount)
{
	size_t fileNameLength = strlen(fileName);

	// Spin up threads 
	while(threadCount > 0)
	{
		char* nameCopy = new char[fileNameLength + 1];
		memcpy(nameCopy, fileName, fileNameLength);
		nameCopy[fileNameLength] = '\0';

		ThreadCreateAndDetach(LogTestWorker, (void*) nameCopy);

		threadCount--;
	}
}

//-----------------------------------------------------------------------------------------------
// Creates the logger instance and returns a reference
//
Logger* Logger::CreateInstance()
{
	if(!s_logger)
	{
		s_logger = new Logger();
	}

	return s_logger;
}

//-----------------------------------------------------------------------------------------------
// Returns the logger instance
//
Logger* Logger::GetInstance()
{
	return s_logger;
}

//-----------------------------------------------------------------------------------------------
// Destroys the logger instance
//
void Logger::DestroyInstance()
{
	delete s_logger;
	s_logger = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Logger thread worker to write into files
//
void Logger::LogThreadWorker(void* userData /*= nullptr */)
{
	Logger* logger = Logger::GetInstance();
	while(logger->IsRunning())
	{
		logger->Flush();
		ThreadSleep(10);
	}

	logger->Flush();

	UNUSED(userData);
}

//-----------------------------------------------------------------------------------------------
// Log test worker thread safe callback. Reads a file and writes to log
//
void Logger::LogTestWorker(void* userData /*= nullptr */)
{
	const char* fileName = (char*) userData;

	// Read the file
	char* fileContents = (char*) FileReadToNewBuffer(fileName);
	size_t threadId = ThreadGetCurrentID();

	// Tokenize into lines
	StringTokenizer tokenizer(fileContents, "\n");
	tokenizer.Tokenize();
	Strings lines = tokenizer.GetTokens();

	// Log each line
	for(size_t lineNum = 0; lineNum < lines.size(); ++lineNum)
	{
		LogTaggedPrintf("logtest", "%d:%d: %s\n", threadId, lineNum, lines[lineNum].c_str());
	}

	LogTaggedPrintf("teststatus", "\n Log Testing success on thread: %d \n", threadId);

	free( (void*) fileName);
	free(fileContents);
}

//-----------------------------------------------------------------------------------------------
// Prints a message with a tag to the log
//
void LogTaggedPrintv(const char* tag, const char* format, va_list args)
{
	const int MAX_LENGTH = 2048;
	char buffer[MAX_LENGTH];

	vsnprintf_s(buffer, MAX_LENGTH, _TRUNCATE, format, args);
	buffer[MAX_LENGTH - 1] = '\0';

	s_logger->AddLogMessage(tag, buffer);
}

//-----------------------------------------------------------------------------------------------
// Flushes the log and writes the log queue to file
//
void LogFlush()
{
	s_logger->Flush();
}

//-----------------------------------------------------------------------------------------------
// Prints a tagged log message to file
//
void LogTaggedPrintf(const char* tag, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogTaggedPrintv(tag, format, args);
	va_end(args);
}

//-----------------------------------------------------------------------------------------------
// Prints log message with the default log tag to file
//
void LogPrintf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogTaggedPrintv(DEFAULT_LOG_TAG, format, args);
	va_end(args);
}

//-----------------------------------------------------------------------------------------------
// Prints log message with the warning tag to file
//
void LogWarningf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogTaggedPrintv(DEFAULT_WARNING_TAG, format, args);
	va_end(args);
}

//-----------------------------------------------------------------------------------------------
// Prints log message with the error tag to file
//
void LogErrorf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogTaggedPrintv(DEFAULT_ERROR_TAG, format, args);
	va_end(args);
}

//-----------------------------------------------------------------------------------------------
// Converts the filters to blacklists
//
void LogShowAll()
{
	s_logger->EnableAll();
}

//-----------------------------------------------------------------------------------------------
// Converts the filters to whitelists
//
void LogHideAll()
{
	s_logger->DisableAll();
}

//-----------------------------------------------------------------------------------------------
// if blacklist, tag will be removed. If whitelist, tag will be added
//
void LogShowTag(const char* tag)
{
	s_logger->EnableTag(tag);
}

//-----------------------------------------------------------------------------------------------
// if whitelist, tag will be removed. If blacklist, tag will be added
//
void LogHideTag(const char* tag)
{
	s_logger->DisableTag(tag);
}

//-----------------------------------------------------------------------------------------------
// Hooks into the log system to callback when something is logged
//
void LogHook(LogCb cb, void* userData /*= nullptr */)
{
	s_logger->AddHook(cb, userData);
}

//-----------------------------------------------------------------------------------------------
// Unplugs from the log system
//
void LogUnhook(LogCb cb, void* userData /*= nullptr */)
{
	s_logger->RemoveHook(cb, userData);
}
