#include "Engine/Profiler/ProfilerReport.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Profiler/ProfilerReportEntry.hpp"
#include "Engine/Profiler/ProfilerSample.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
ProfilerReport::ProfilerReport()
{

}

//-----------------------------------------------------------------------------------------------
// Destructor
//
ProfilerReport::~ProfilerReport()
{
	delete m_root;
}

//-----------------------------------------------------------------------------------------------
// Returns the total frame time
//
double ProfilerReport::GetTotalFrameTime() const
{
	return m_root->m_totalTime;
}

//-----------------------------------------------------------------------------------------------
// Generates a tree report from the root of the samples
//
void ProfilerReport::GenerateTreeFromFrame(ProfilerSample* root)
{
	m_root = new ProfilerReportEntry(root->id);
	m_root->PopulateTree(root);
}

//-----------------------------------------------------------------------------------------------
// Generates a flat report from the root of the samples
//
void ProfilerReport::GenerateFlatFromFrame(ProfilerSample* root)
{
	m_root = new ProfilerReportEntry(root->id);
	m_root->PopulateFlat(root);

	m_root->CollectDataFromNode(root);
}

//-----------------------------------------------------------------------------------------------
// Sorts the flat view by total time
//
void ProfilerReport::SortByTotalTime()
{
	m_root->SortByTotalTime();
}

//-----------------------------------------------------------------------------------------------
// Sorts the tree view by total time
//
void ProfilerReport::SortBySelfTime()
{
	m_root->SortBySelfTime();
}

//-----------------------------------------------------------------------------------------------
// Prints the report to console
//
void ProfilerReport::PrintReportToConsole()
{
	m_root->PrintToConsole();
}

//-----------------------------------------------------------------------------------------------
// Returns the report text as a formatted string (Ready to render)
//
std::string ProfilerReport::GetFormattedString() const
{
	return m_root->GetFormattedString();
}

//-----------------------------------------------------------------------------------------------
// Renders the report to debug logger
//
void ProfilerReport::DebugRender() const
{
	m_root->DebugRender();
}
