#include "Engine/Profiler/ProfileLogScope.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Profiler/Profiler.hpp"

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
ProfileLogScope::ProfileLogScope(char tag[])
{
	Profiler::Push(tag);
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
ProfileLogScope::~ProfileLogScope()
{
	Profiler::Pop();
}
