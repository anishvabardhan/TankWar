#pragma once
#include <string>

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class ProfilerReportEntry;
struct ProfilerSample;

//-----------------------------------------------------------------------------------------------
class ProfilerReport
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	ProfilerReport();
	~ProfilerReport();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	double	GetTotalFrameTime() const;
	
	//-----------------------------------------------------------------------------------------------
	// Methods
	void		GenerateTreeFromFrame( ProfilerSample* root );
	void		GenerateFlatFromFrame( ProfilerSample* root );
	void		SortByTotalTime();
	void		SortBySelfTime();
	void		PrintReportToConsole();
	std::string	GetFormattedString() const;
	void		DebugRender() const;

	//-----------------------------------------------------------------------------------------------
	// Members
	ProfilerReportEntry*	m_root;
};

