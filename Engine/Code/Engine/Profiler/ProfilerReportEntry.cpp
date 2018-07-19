#include "Engine/Profiler/ProfilerReportEntry.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Profiler/ProfilerSample.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/DebugRenderUtils.hpp"
#include "Engine/Console/DevConsole.hpp"

//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
// Constructor
//
ProfilerReportEntry::ProfilerReportEntry(const char* id)
{
	m_id = id;
}

//-----------------------------------------------------------------------------------------------
// Destructor 
//
ProfilerReportEntry::~ProfilerReportEntry()
{
	DestroyRecursive();
}

//-----------------------------------------------------------------------------------------------
// Returns the formatted string
//
std::string ProfilerReportEntry::GetFormattedString() const
{
	std::string formattedText;
	
	if(!m_parent)
	{
		std::string debugHeaderString = Stringf("%*s%-*s %-8s %-14s %-8s %-14s",
			m_indent, "",
			40 - m_indent, "FUNCTION",
			"CALLS",
			"TOTAL TIME",
			"TOTAL%",
			"SELF TIME");
		
		formattedText.append(debugHeaderString + "\n");
	}

	std::string debugString = Stringf("%*s%-*s %-8d (%-8f)ms %-8f (%-8f)ms", 
		m_indent, "", 
		40 - m_indent, m_id.c_str(), 
		m_callCount, 
		m_totalTime, 
		m_percentTime,
		m_selfTime );
	formattedText.append(debugString + "\n");

	for(ProfilerReportEntry* child : m_children)
	{
		formattedText.append(child->GetFormattedString());
	}

	return formattedText;
}

//-----------------------------------------------------------------------------------------------
// Populates the report entry as a tree view of the data
//
void ProfilerReportEntry::PopulateTree(const ProfilerSample* node)
{
	CollectDataFromNode(node);

	double childrenTime = 0.0;
	for(ProfilerSample* child : node->children)
	{
		ProfilerReportEntry* entry = CreateOrGetChild(child->id);
		entry->PopulateTree(child);
		childrenTime += entry->m_totalTime;
	}

	double rootTime = node->GetRootTotalTime();
	m_selfTime = m_totalTime - childrenTime;
	m_percentTime = m_totalTime / rootTime;
}

//-----------------------------------------------------------------------------------------------
// Populates the report entry as a flat view of the data
//
void ProfilerReportEntry::PopulateFlat(const ProfilerSample* node)
{
	for(ProfilerSample* child : node->children)
	{
		ProfilerReportEntry* entry = CreateOrGetChild(child->id);
		entry->CollectDataFromNode(child);
		PopulateFlat(child);
	}
}
//-----------------------------------------------------------------------------------------------
// Collects data from the node and populates this
//
void ProfilerReportEntry::CollectDataFromNode(const ProfilerSample* node)
{
	m_callCount++;
	
	m_totalTime += node->GetElapsedSeconds();
	double rootTime = node->GetRootTotalTime();
	double childrenTime = node->GetChildrenTotalTime();
	m_selfTime = m_totalTime - childrenTime;
	m_percentTime = m_totalTime / rootTime;

	if(m_parent)
	{
		m_indent = m_parent->m_indent + 1;
	}
}

//-----------------------------------------------------------------------------------------------
// Creates a child or returns a reference to an existing child
//
ProfilerReportEntry* ProfilerReportEntry::CreateOrGetChild(const char* id)
{
	ProfilerReportEntry* entry = FindEntry(id);
	if (entry == nullptr) 
	{
		entry = new ProfilerReportEntry(id); 
		entry->m_parent = this; 
		m_children.push_back(entry); 
	}
	return entry; 
}

//-----------------------------------------------------------------------------------------------
// Finds the entry in the children list and returns a reference, returns nullptr if not found
//
const ProfilerReportEntry* ProfilerReportEntry::FindEntry(const char* id) const
{
	for(ProfilerReportEntry* child : m_children)
	{
		if(child->m_id == id)
		{
			return child;
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
// Finds the entry in the children list and returns a reference, returns nullptr if not found
//
ProfilerReportEntry* ProfilerReportEntry::FindEntry(const char* id)
{
	for(ProfilerReportEntry* child : m_children)
	{
		if(child->m_id == id)
		{
			return child;
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
// Destroys the tree recursively
//
void ProfilerReportEntry::DestroyRecursive()
{
	for(ProfilerReportEntry* child : m_children)
	{
		delete child;
		child = nullptr;
	}

	m_children.clear();
}

//-----------------------------------------------------------------------------------------------
// Prints the entry to console
//
void ProfilerReportEntry::PrintToConsole()
{
	if(!m_parent)
	{
		std::string debugHeaderString = Stringf("%*s%-*s %-8s %-14s %-8s %-14s",
			m_indent, "",
			45 - m_indent, "FUNCTION",
			"CALLS",
			"TOTAL TIME",
			"TOTAL%",
			"SELF TIME");
		ConsolePrintf("%s", debugHeaderString.c_str());
	}
	std::string debugString = Stringf("%*s%-*s %-8d (%-8f)ms %-8f (%-8f)ms", 
		m_indent, "", 
		45 - m_indent, m_id.c_str(), 
		m_callCount, 
		m_totalTime, 
		m_percentTime,
		m_selfTime );
	ConsolePrintf("%s", debugString.c_str());

	for(ProfilerReportEntry* child : m_children)
	{
		child->PrintToConsole();
	}
}

//-----------------------------------------------------------------------------------------------
// Renders the entry to the log
//
void ProfilerReportEntry::DebugRender()
{
	std::string debugString = Stringf("%*s%-*s %-8d (%-8f)s (%-8f)s", m_indent, "", 45 - m_indent, m_id.c_str(), m_callCount, m_totalTime, m_selfTime );
	DebugRenderLogf(0.f, "%s", debugString.c_str());

	for(ProfilerReportEntry* child : m_children)
	{
		child->DebugRender();
	}
}

//-----------------------------------------------------------------------------------------------
// Sorts the entries recursively by total time
//
void ProfilerReportEntry::SortByTotalTime()
{
	for(int indexI = 0; indexI < m_children.size(); ++indexI)
	{
		double totalTimeI = m_children[indexI]->GetTotalTime();

		for(int indexJ = indexI + 1; indexJ < m_children.size(); ++indexJ)
		{
			double totalTimeJ = m_children[indexJ]->GetTotalTime();
			if( totalTimeI > totalTimeJ )
			{
				std::swap(m_children[indexI], m_children[indexJ]);
				totalTimeI = totalTimeJ;
			}
		}
	}

	for(ProfilerReportEntry* child : m_children)
	{
		child->SortByTotalTime();
	}
}

//-----------------------------------------------------------------------------------------------
// Sorts the entries recursively by total time
//
void ProfilerReportEntry::SortBySelfTime()
{
	for(int indexI = 0; indexI < m_children.size(); ++indexI)
	{
		double selfTimeI = m_children[indexI]->GetSelfTime();

		for(int indexJ = indexI + 1; indexJ < m_children.size(); ++indexJ)
		{
			double selfTimeJ = m_children[indexJ]->GetSelfTime();
			if( selfTimeI > selfTimeJ )
			{
				std::swap(m_children[indexI], m_children[indexJ]);
				selfTimeI = selfTimeJ;
			}
		}
	}

	for(ProfilerReportEntry* child : m_children)
	{
		child->SortBySelfTime();
	}
}
