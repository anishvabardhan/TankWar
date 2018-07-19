#pragma once
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
// Forward Declarations
struct ProfilerSample;

//-----------------------------------------------------------------------------------------------
class ProfilerReportEntry
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	ProfilerReportEntry( const char* id );
	~ProfilerReportEntry();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
			double					GetTotalTime() const { return m_totalTime; }
			double					GetSelfTime() const { return m_selfTime; }
			double					GetFunctionTime() const { return m_selfTime; }
			std::string				GetFormattedString() const;
	
	//-----------------------------------------------------------------------------------------------
	// Methods
			void					PopulateTree( const ProfilerSample* node );
			void					PopulateFlat( const ProfilerSample* node );
			void					CollectDataFromNode( const ProfilerSample* node );
			ProfilerReportEntry*	CreateOrGetChild( const char* id );
	const	ProfilerReportEntry*	FindEntry( const char* id ) const;
			ProfilerReportEntry*	FindEntry( const char* id );
			void					DestroyRecursive();
			void					PrintToConsole();
			void					DebugRender();
			void					SortByTotalTime();
			void					SortBySelfTime();

	//-----------------------------------------------------------------------------------------------
	// Members
	std::string									m_id; 
	int											m_indent;
	int											m_callCount = 0; 
	double										m_totalTime = 0.0; // inclusive time; 
	double										m_selfTime = 0.0;  // exclusive time
	double										m_percentTime;
	ProfilerReportEntry*						m_parent; 
	std::vector<ProfilerReportEntry*>			m_children; 
};

