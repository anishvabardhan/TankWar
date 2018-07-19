#pragma once
#include <cstdint>
#include <string>

//-----------------------------------------------------------------------------------------------
class Time
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	Time();
	~Time(){}
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	
	//-----------------------------------------------------------------------------------------------
	// Methods
	static	uint64_t	GetPerformanceCounter();
	static	double		HpcToSeconds( const uint64_t hpc );
	static	uint64_t	SecondsToHpc( float seconds );
	static	void		CreateInstance();
	static  void		DestroyInstance();
	static  std::string GetSysTimeStamp();
	
	//-----------------------------------------------------------------------------------------------
	// Members
private:
	uint64_t	m_frequency;
	double		m_secondsPerCount;
};


