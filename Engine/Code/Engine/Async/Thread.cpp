#include "Engine/Async/Thread.hpp"
//-----------------------------------------------------------------------------------------------
// Game Includes

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Engine Includes

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Standard Includes
#include "Engine/Core/Platform/Win32.hpp"
#include "Engine/Core/EngineCommon.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Defines
#define MS_VC_EXCEPTION				(0x406d1388)
#define DEFAULT_THREAD_STACK_SIZE	(1 * MB)

//-----------------------------------------------------------------------------------------------
#pragma pack(push, 8)
struct THREADNAME_INFO // Structure packed 8 bytes to ensure usage with winapi call
{
	DWORD type;            // must be 0x1000
	const char *name;      // name
	DWORD thread_id;      // -1 for calling thread
	DWORD flags;         // must be 0, reserved for future use
};
#pragma pack(pop)

//-----------------------------------------------------------------------------------------------
struct ThreadStartArgs
{
	const char* name;
	ThreadCb	function;
	void*		userData;
};

//-----------------------------------------------------------------------------------------------
// Thread start routine used by the thread creation call
//
static DWORD WINAPI ThreadEntryPoint( void* arg )
{
	// Set the name of the thread for easier debugging
	ThreadStartArgs* args = (ThreadStartArgs*) arg;
	ThreadSetName(args->name);

	// Get the data from the passed arguments
	ThreadCb cb = args->function;
	void* data = args->userData;

	// Free the initial memory used
	delete args;

	// Call the callback with the data 
	cb(data);

	return 0;
}

//-----------------------------------------------------------------------------------------------
// Creates the Thread and returns the handle
//
ThreadHandle ThreadCreate(const char* name, ThreadCb cb, size_t stackSize /*= 0*/, void* userData /*= nullptr */)
{
	ThreadStartArgs* args = new ThreadStartArgs();
	args->name = name;
	args->function = cb;
	args->userData = userData;

	// If stack size is not mentioned, default to (1 MB)
	if(stackSize == 0)
	{
		stackSize = DEFAULT_THREAD_STACK_SIZE;
	}

	DWORD id = 0;
	ThreadHandle handle = (ThreadHandle) ::CreateThread(NULL, stackSize, ThreadEntryPoint, args, 0, &id);

	return handle;
}

//-----------------------------------------------------------------------------------------------
// Creates the Thread and returns the handle
//
ThreadHandle ThreadCreate(const char* name, ThreadCb cb, void* userData /*= nullptr */)
{
	return ThreadCreate(name, cb, 0, userData);
}

//-----------------------------------------------------------------------------------------------
// Creates the Thread and returns the handle
//
ThreadHandle ThreadCreate(ThreadCb cb, void* userData /*= nullptr */)
{
	return ThreadCreate(nullptr, cb, 0, userData);
}

//-----------------------------------------------------------------------------------------------
// Causes a blocking wait until the thread is done then closes the handle
//
void ThreadJoin(ThreadHandle handle)
{
	if(handle != nullptr)
	{
		::WaitForSingleObject(handle, INFINITE);
		::CloseHandle(handle);
	}
}

//-----------------------------------------------------------------------------------------------
// Detaches the thread from the program and continues until its end
//
void ThreadDetach(ThreadHandle handle)
{
	if(handle != nullptr)
	{
		::CloseHandle(handle);
	}
}

//-----------------------------------------------------------------------------------------------
// Creates the thread and detaches it
//
void ThreadCreateAndDetach(const char* name, ThreadCb cb, size_t stackSize /*= 0*/, void* userData /*= nullptr */)
{
	ThreadHandle handle = ThreadCreate(name, cb, stackSize, userData);
	ThreadDetach(handle);
}

//-----------------------------------------------------------------------------------------------
// Creates the thread and detaches it
//
void ThreadCreateAndDetach(const char* name, ThreadCb cb, void* userData /*= nullptr */)
{
	ThreadCreateAndDetach(name, cb, 0, userData);
}

//-----------------------------------------------------------------------------------------------
// Creates the thread and detaches it
//
void ThreadCreateAndDetach(ThreadCb cb, void* userData /*= nullptr */)
{
	ThreadCreateAndDetach(nullptr, cb, 0, userData);
}

//-----------------------------------------------------------------------------------------------
// Returns the ID of the current thread
//
uintptr_t ThreadGetCurrentID()
{
	return (uintptr_t) ::GetCurrentThreadId();
}

//-----------------------------------------------------------------------------------------------
// Returns the ID of the thread specified
//
uintptr_t ThreadGetID(ThreadHandle handle)
{
	return (uintptr_t) ::GetThreadId(handle);
}

//-----------------------------------------------------------------------------------------------
// Puts the current thread to sleep for the specified time in milliseconds
//
void ThreadSleep(int ms)
{
	::Sleep( (DWORD) ms );
}

//-----------------------------------------------------------------------------------------------
// Switches the thread if there are others on the queue
//
void ThreadYield()
{
	::SwitchToThread();
}

//-----------------------------------------------------------------------------------------------
// Sets the name of the thread for debugging in Visual Studio
//
void ThreadSetName(const char* name)
{
	if(name == nullptr)
	{
		return;
	}

	uintptr_t id = ThreadGetCurrentID();
	if(id != 0)
	{
		THREADNAME_INFO info;
		info.type = 0x1000;
		info.name = name;
		info.thread_id = (DWORD) id;
		info.flags = 0;
	
		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*) (&info) );
		}

		__except (EXCEPTION_CONTINUE_EXECUTION)
		{

		}
	}
}
