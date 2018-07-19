#include "Engine/Profiler/ProfilerSample.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
// Constructor
//
ProfilerSample::ProfilerSample( char name[] )
{
	strcpy_s(id, name);
	Start();
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
ProfilerSample::~ProfilerSample()
{
	DestroyRecursive();
}

//-----------------------------------------------------------------------------------------------
// Starts the measurement
//
void ProfilerSample::Start()
{
	startHpc = Time::GetPerformanceCounter();
}

//-----------------------------------------------------------------------------------------------
// Finishes the measurement -> Stores the hpc
//
void ProfilerSample::Finish()
{
	endHpc = Time::GetPerformanceCounter();
}

//-----------------------------------------------------------------------------------------------
// Adds a child to the list of children
//
void ProfilerSample::AddChild(ProfilerSample* child)
{
	children.push_back(child);
}

//-----------------------------------------------------------------------------------------------
// Recursively destroys the sample and its children
//
void ProfilerSample::DestroyRecursive()
{
	// Delete the children
	for(ProfilerSample* child : children)
	{
		delete child;
		child = nullptr;
	}

	if(children.size())
		children.clear();	
}

//-----------------------------------------------------------------------------------------------
// Returns the total elapsed time for this sample
//
double ProfilerSample::GetElapsedSeconds() const
{
	uint64_t elapsedHpc = endHpc - startHpc;
	return Time::HpcToSeconds(elapsedHpc);
}

//-----------------------------------------------------------------------------------------------
// Returns the total elapsed time of the root
//
double ProfilerSample::GetRootTotalTime() const
{
	if(!parent)
	{
		return GetElapsedSeconds();
	}
	else
	{
		return parent->GetRootTotalTime();
	}
}

//-----------------------------------------------------------------------------------------------
// Returns the total time of the immediate children
//
double ProfilerSample::GetChildrenTotalTime() const
{
	double childrenTime = 0.0;
	for(ProfilerSample* sample : children)
	{
		childrenTime += sample->GetElapsedSeconds();
	}

	return childrenTime;
}
