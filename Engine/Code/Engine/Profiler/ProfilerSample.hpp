#pragma once
#include <stdint.h>
#include <vector>

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Clock;

//-----------------------------------------------------------------------------------------------
struct ProfilerSample
{
	//-----------------------------------------------------------------------------------------------
	// Constructors
	ProfilerSample( char name[] );
	~ProfilerSample();

	//-----------------------------------------------------------------------------------------------
	// Methods
	void	Start();
	void	Finish();
	void	AddChild( ProfilerSample* child );
	void	DestroyRecursive();
	double	GetElapsedSeconds() const;
	double	GetRootTotalTime() const;
	double	GetChildrenTotalTime() const;

	//-----------------------------------------------------------------------------------------------
	// Members
	char							id[64];
	uint64_t						startHpc;
	uint64_t						endHpc;

	ProfilerSample*					parent = nullptr;
	std::vector<ProfilerSample*>	children;
};



