#pragma once
#include "Engine/Core/Types.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations


//-----------------------------------------------------------------------------------------------
// Types
struct ThreadHandleType;
typedef ThreadHandleType* ThreadHandle;
//typedef void* ThreadHandle;

// Thread callback
typedef void (*ThreadCb)(void* userData);

//-----------------------------------------------------------------------------------------------
// Thread Management functions

// Creation helpers
ThreadHandle	ThreadCreate( const char* name, ThreadCb cb, size_t stackSize = 0, void* userData = nullptr );
ThreadHandle	ThreadCreate( const char* name, ThreadCb cb, void* userData = nullptr );
ThreadHandle	ThreadCreate( ThreadCb cb, void* userData = nullptr );

// Join and Detach Helpers
void			ThreadJoin( ThreadHandle handle );
void			ThreadDetach( ThreadHandle handle );

// Create and Detach helpers
void			ThreadCreateAndDetach( const char* name, ThreadCb cb, size_t stackSize = 0, void* userData = nullptr );
void			ThreadCreateAndDetach( const char* name, ThreadCb cb, void* userData = nullptr );
void			ThreadCreateAndDetach( ThreadCb cb, void* userData = nullptr );

// General Helpers
uintptr_t		ThreadGetCurrentID();
uintptr_t		ThreadGetID( ThreadHandle handle );

//-----------------------------------------------------------------------------------------------
// Thread control functions
void			ThreadSleep( int ms );
void			ThreadYield();

//-----------------------------------------------------------------------------------------------
// Thread debug functions
void			ThreadSetName( const char* name );
