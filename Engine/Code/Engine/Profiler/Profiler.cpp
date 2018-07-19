#include "Engine/Profiler/Profiler.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Console/CommandDefinition.hpp"
#include "Engine/Profiler/ProfilerReport.hpp"
#include "Engine/Console/Command.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/UI/Canvas.hpp"
#include "Engine/UI/Widgets/Widget_Textbox.hpp"
#include "Engine/UI/Widgets/Widget_AreaGraph.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Static globals
static Profiler* g_profiler = nullptr;

#if defined(ENGINE_ENABLE_PROFILING)
//-----------------------------------------------------------------------------------------------
// Constructor
//
Profiler::Profiler()
{
	m_prevStacks.resize(PROFILER_HISTORY_SIZE);
	
	m_canvas = new Canvas();
	m_reportBoxRef = m_canvas->AddTextBox("reportBox", Vector2::ZERO, Vector2(1.f, 0.7f), "test");
	m_controlsBoxRef = m_canvas->AddTextBox("controlsBox", Vector2(0.f, 0.72f), Vector2(0.4f, 0.82f), "controlsBox");
	m_statusBoxRef = m_canvas->AddTextBox("statusBox", Vector2(0.f, 0.84f), Vector2(0.4f, 0.94f), "statusBox");
	
	m_graphBoxRef = m_canvas->AddAreaGraph("usageGraph", Vector2(0.5f, 0.72f), Vector2(1.f, 0.94f), {});
	m_graphBoxRef->SetOnClick(CPUGraphClickListener);
	m_graphBoxRef->SetThresholds(18.f, 22.f); // FPS: 55, 45

	COMMAND("profiler_pause", PauseProfilerCommand, "Pauses the profiler if running");
	COMMAND("profiler_resume", ResumeProfilerCommand, "Resumes the profiler if paused");
	COMMAND("profiler", ProfilerCommand, "Shows the profiler view");
	COMMAND("profiler_report", ProfilerReportCommand, "Prints last frame report to console (tree|flat)");
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
Profiler::~Profiler()
{
	delete m_canvas;
	m_canvas = nullptr;

	ClearReports();

	for(ProfilerSample* sample : m_prevStacks)
	{
		delete sample;
		sample = nullptr;
	}
	m_prevStacks.clear();

	if(m_activeNode)
	{
		delete m_activeNode;
		m_activeNode = nullptr;
	}
}

//-----------------------------------------------------------------------------------------------
// Creates a sample and pushes it to the stack
//
void Profiler::PushProfile(char id[])
{
	if(m_isPaused)
	{
		return;
	}

	ProfilerSample* sample = CreateSample(id);

	if(m_activeNode == nullptr)
	{
		m_activeNode = sample;
	}
	else
	{
		sample->parent = m_activeNode;
		m_activeNode->AddChild(sample);
		m_activeNode = sample;
	}
}

//-----------------------------------------------------------------------------------------------
// Pops the stack 
//
void Profiler::PopProfile()
{
	if(m_isPaused)
	{
		return; 
	}

	GUARANTEE_OR_DIE(m_activeNode != nullptr, "Called Pop without push");

	m_activeNode->Finish();
	m_activeNode = m_activeNode->parent;
}

//-----------------------------------------------------------------------------------------------
// Pops the profiler stack
//
STATIC void Profiler::Pop()
{
	g_profiler->PopProfile();
}

//-----------------------------------------------------------------------------------------------
// Marks the frame start in the profiler
//
void Profiler::MarkFrame()
{
	g_profiler->StartProfileFrame();
}

//-----------------------------------------------------------------------------------------------
// Closes the profiler screen
//
void Profiler::Close()
{
	g_profiler->m_isProfilerOpen = false;
}

//-----------------------------------------------------------------------------------------------
// Opens the profiler screen
//
void Profiler::Open()
{
	g_profiler->m_isProfilerOpen = true;
}

//-----------------------------------------------------------------------------------------------
// Handles click input when the user clicks on the graph
//
void Profiler::CPUGraphClickListener(int selectedIndex, MouseButton buttonCode)
{
	switch (buttonCode)
	{
	case MOUSE_LMB:
		g_profiler->PauseProfiler();
		g_profiler->m_selectedFrame = selectedIndex;
		break;

	case MOUSE_RMB:
		g_profiler->ResumeProfiler();
		g_profiler->m_selectedFrame = 0;
		break;
	}
}

//-----------------------------------------------------------------------------------------------
// Console command to pause profiler
//
bool Profiler::PauseProfilerCommand(Command& cmd)
{
	g_profiler->PauseProfiler();

	UNUSED(cmd);
	return true;
}

//-----------------------------------------------------------------------------------------------
// Console command to resume profiler
//
bool Profiler::ResumeProfilerCommand(Command& cmd)
{
	g_profiler->ResumeProfiler();

	UNUSED(cmd);
	return true;
}

//-----------------------------------------------------------------------------------------------
// Console command to view the profiler reports
//
bool Profiler::ProfilerCommand(Command& cmd)
{
	if(g_profiler->IsProfilerOpen())
	{
		g_profiler->Close();
	}
	else
	{
		g_profiler->Open();
	}

	UNUSED(cmd);
	return true;
}

//-----------------------------------------------------------------------------------------------
// Console command to print the last frame's report
//
bool Profiler::ProfilerReportCommand(Command& cmd)
{
	std::string reportType = cmd.GetNextString();

	if(reportType == "tree" || reportType == "")
	{
		g_profiler->PrintLastFrameToConsole(REPORT_TYPE_TREE);
	}
	else if(reportType == "flat")
	{
		g_profiler->PrintLastFrameToConsole(REPORT_TYPE_FLAT);
	}
	else
	{
		ConsolePrintf("'%s' Unsupported report type", reportType.c_str());
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
// Creates a sample and returns it
//
ProfilerSample* Profiler::CreateSample(char id[])
{
	ProfilerSample* sample = new ProfilerSample(id);
	return sample;
}

//-----------------------------------------------------------------------------------------------
// Marks the start of a frame
//
void Profiler::StartProfileFrame()
{
	if(m_activeNode != nullptr)
	{
		if(m_prevStacks[m_prevFrameIndex] != nullptr)
		{
			delete m_prevStacks[m_prevFrameIndex];
		}
		m_prevStacks[m_prevFrameIndex] = m_activeNode;
		PopProfile();
		m_prevFrameIndex = (m_prevFrameIndex + 1) % PROFILER_HISTORY_SIZE;
	
		GUARANTEE_OR_DIE(m_activeNode == nullptr, "Pushed without popping");
	}

	if(m_isReadyToPause)
	{
		m_isPaused = true;
		m_isReadyToPause = false;
		return;
	}

	if(m_isReadyToResume)
	{
		m_isPaused = false;
		m_isReadyToResume = false;
	}

	PushProfile(PROFILER_START_FRAME_TEXT);
}

//-----------------------------------------------------------------------------------------------
// Pauses the profiler 
//
void Profiler::PauseProfiler()
{
	m_isReadyToPause = true;
	m_isReadyToResume = false;
}

//-----------------------------------------------------------------------------------------------
// Resumes the profiler
//
void Profiler::ResumeProfiler()
{
	m_isReadyToPause = false;
	m_isReadyToResume = true;
}

//-----------------------------------------------------------------------------------------------
// Returns the previous frame trees in order. Most recent frame at 0
//
std::vector<ProfilerSample*> Profiler::GetOrderedPreviousFrames() 
{
	std::vector<ProfilerSample*> frames(PROFILER_HISTORY_SIZE);

	int frameIndex = 0;
	for(int orderIndex = PROFILER_HISTORY_SIZE - 1; orderIndex >= 0; --orderIndex)
	{
		ProfilerSample* sample = GetPreviousFrame(frameIndex);
		frames[orderIndex] = sample;
		frameIndex++;
	}

	return frames;
}

//-----------------------------------------------------------------------------------------------
// Updates the profiler view if its open
//
void Profiler::Update(float deltaSeconds)
{
	if(!m_isProfilerOpen)
	{
		return; // No need to generate/update reports if the profiler is closed
	}

	ProcessInput();
	ProcessMouseInput();

	UpdateReportBoxText();
	UpdateStatusBoxText();
	UpdateControlsBoxText();
	UpdateGraphBoxValues();
	m_canvas->Update(deltaSeconds);

	UNUSED(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Updates the report text box with the formatted string
//
void Profiler::UpdateReportBoxText()
{
	ClearReports();

	// Most recent frame is at 0
	std::vector<ProfilerSample*> prevFrames = GetOrderedPreviousFrames();

	for(ProfilerSample* sample : prevFrames)
	{
		if(sample == nullptr)
		{
			break;
		}

		ProfilerReport* report = new ProfilerReport();

		switch (m_reportViewType)
		{
		case REPORT_TYPE_FLAT:
			report->GenerateFlatFromFrame(sample);
			break;
		case REPORT_TYPE_TREE:
			report->GenerateTreeFromFrame(sample);
			break;
		default:
			GUARANTEE_OR_DIE(false, "Wrong report frame type");
			break;
		}
		
		switch (m_reportSortMode)
		{
		case SORT_BY_NONE:			/* DO NOTHING */ 				break;
		case SORT_BY_TOTAL_TIME:	report->SortByTotalTime();		break;
		case SORT_BY_SELF_TIME:		report->SortBySelfTime(); 		break;
		default:
			GUARANTEE_OR_DIE(false, "Bad report sort mode");
			break;
		}

		m_reports.push_back(report);
	}

	ProfilerReport* report = m_reports[m_selectedFrame];
	m_reportBoxRef->SetText(report->GetFormattedString());
}

//-----------------------------------------------------------------------------------------------
// Updates the status box text widget
//
void Profiler::UpdateStatusBoxText()
{
	double frameTime = m_reports[m_selectedFrame]->GetTotalFrameTime();
	double fps = 0.0;

	if(m_isPaused)
	{
		fps = 1.0 / frameTime;
	}
	else
	{
		for(int index = 0; index < 60; ++index) // Assuming 60 frames a second to get the average value
		{
			fps += m_reports[index]->GetTotalFrameTime();
		}

		fps = 60.0 * 1.0 / fps;
	}
	
	std::string fpsString = Stringf("FPS: %0.2f", fps) + "\n";
	std::string frameTimeStr = Stringf("Frame Time: %0.4f ms", frameTime * 1000.0) + "\n";
	m_statusBoxRef->SetText(fpsString + frameTimeStr);
}

//-----------------------------------------------------------------------------------------------
// Updates the controls box text widget
//
void Profiler::UpdateControlsBoxText()
{
	std::string ReportViewText = Stringf("Toggle ReportView [V]: %s\n", m_reportViewType == REPORT_TYPE_TREE ? "TREE VIEW" : "FLAT VIEW");
	std::string MouseText = Stringf("Enable/Disable Mouse [M]: %s\n", m_hasMouseControl ? "Enabled" : "Disabled");

	std::string sortModeStr;
	switch (m_reportSortMode)
	{
	case SORT_BY_NONE:			sortModeStr = "NONE";		break;
	case SORT_BY_TOTAL_TIME:	sortModeStr = "TOTAL TIME"; break;
	case SORT_BY_SELF_TIME:		sortModeStr = "SELF TIME";	break;
	default:					sortModeStr = "BAD STRING";	break;	
	}

	std::string SortText = Stringf("Toggle Sort modes [L]: %s \n", sortModeStr.c_str());
	std::string controlsText =  ReportViewText + MouseText + SortText;
	m_controlsBoxRef->SetText(controlsText);
}

//-----------------------------------------------------------------------------------------------
// Updates the graph values on the graph widget
//
void Profiler::UpdateGraphBoxValues()
{
	std::vector<float> timeValues(PROFILER_HISTORY_SIZE);

	for(int index = 0; index < PROFILER_HISTORY_SIZE; ++index)
	{
		timeValues[index] = (float) m_reports[index]->GetTotalFrameTime() * 1000.f;
	}

	m_graphBoxRef->SetValues(timeValues);
}

//-----------------------------------------------------------------------------------------------
// Renders the profiler report
//
void Profiler::Render() const
{
	if(!m_isProfilerOpen)
	{
		return; // No need to show reports if the profiler is closed
	}

	m_canvas->Render();
}

//-----------------------------------------------------------------------------------------------
// Prints the last frame's report to console
//
void Profiler::PrintLastFrameToConsole(ReportType type)
{
	ProfilerReport report;
	ProfilerSample* prevFrame = GetPreviousFrame(0);
	switch (type)
	{
	case REPORT_TYPE_FLAT:
		report.GenerateFlatFromFrame(prevFrame);
		report.PrintReportToConsole();
		break;

	case REPORT_TYPE_TREE:
		report.GenerateTreeFromFrame(prevFrame);
		report.PrintReportToConsole();
		break;

	default:
		GUARANTEE_OR_DIE(false, "Unsupported report type");
	}
}

//-----------------------------------------------------------------------------------------------
// Destroys the reports
//
void Profiler::ClearReports()
{
	for(ProfilerReport* report : m_reports)
	{
		delete report;
	}

	m_reports.clear();
}

//-----------------------------------------------------------------------------------------------
// Processes input keys
//
void Profiler::ProcessInput()
{
	InputSystem* input = InputSystem::GetInstance();
	
	if(input->WasKeyJustPressed(KEYCODE_V))
	{
		m_reportViewType = (ReportType) ((m_reportViewType + 1) % NUM_REPORT_TYPES);
	}

	if(input->WasKeyJustPressed(KEYCODE_L))
	{
		m_reportSortMode = (ReportSortMode) ((m_reportSortMode + 1) % NUM_SORT_MODES);
	}

	if(input->WasKeyJustPressed(KEYCODE_M))
	{
		
		m_hasMouseControl = !m_hasMouseControl;
		m_canvas->SetMouseInput(m_hasMouseControl);
		if(m_hasMouseControl)
		{
			InputSystem::LockMouseToWindow(false);
			InputSystem::ShowCursor(true);
			InputSystem::GetInstance()->SetMouseMode(MOUSE_ABSOLUTE);
		}
		else
		{
			InputSystem::LockMouseToWindow(true);
			InputSystem::ShowCursor(false);
			InputSystem::GetInstance()->SetMouseMode(MOUSE_RELATIVE);
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Processes mouse inputs if the profiler has control 
//
void Profiler::ProcessMouseInput()
{

}

//-----------------------------------------------------------------------------------------------
// Returns the top node
// 
ProfilerSample* Profiler::GetTop() const
{
	return m_activeNode;
}

//-----------------------------------------------------------------------------------------------
// Returns true if the profiler is paused
// 
bool Profiler::IsPaused() const
{
	return m_isPaused;
}

//-----------------------------------------------------------------------------------------------
// Returns true if the profiler is open
// 
bool Profiler::IsProfilerOpen() const
{
	return m_isProfilerOpen;
}

//-----------------------------------------------------------------------------------------------
// Returns the n-th previous frame, 0 meaning immediate previous
//
ProfilerSample* Profiler::GetPreviousFrame(int skipCount /*= 0 */)
{
	if(m_prevStacks.size() <= 0 && skipCount >= 0)
	{
		return nullptr; 
	}

	int actualIndex = ((int) m_prevFrameIndex - 1) - skipCount;
	actualIndex = ( PROFILER_HISTORY_SIZE + ( actualIndex % PROFILER_HISTORY_SIZE ) ) % PROFILER_HISTORY_SIZE;
	return m_prevStacks[actualIndex];
}

//-----------------------------------------------------------------------------------------------
// Returns the n-th previous frame, 0 meaning immediate previous
//
const ProfilerSample* Profiler::GetPreviousFrame(int skipCount /*= 0 */) const
{
	if(m_prevStacks.size() <= 0 && skipCount >= 0)
	{
		return nullptr; 
	}

	skipCount = ClampInt(skipCount, 0, PROFILER_HISTORY_SIZE);
	int actualIndex = ((int) m_prevFrameIndex - 1) - skipCount;
	if(actualIndex < 0)
	{
		actualIndex = PROFILER_HISTORY_SIZE - skipCount;
	}

	return m_prevStacks[actualIndex];
}

//-----------------------------------------------------------------------------------------------
// Creates the instance 
//
STATIC Profiler* Profiler::CreateInstance()
{
	if(!g_profiler)
	{
		g_profiler = new Profiler();
	}

	return g_profiler;
}

//-----------------------------------------------------------------------------------------------
// Destroys the instance
//
STATIC void Profiler::DestroyInstance()
{
	delete g_profiler;
	g_profiler = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Returns the instance
//
STATIC Profiler* Profiler::GetInstance()
{
	return g_profiler;
}

//-----------------------------------------------------------------------------------------------
// Pushes a new sample
//
STATIC void Profiler::Push(char id[])
{
	g_profiler->PushProfile(id);
}

//-----------------------------------------------------------------------------------------------
// Starts up the profiler
//
void ProfilerStartup()
{
	Profiler::CreateInstance();
}

//-----------------------------------------------------------------------------------------------
// Shuts down the profiler
//
void ProfilerShutdown()
{
	Profiler::DestroyInstance();
}

#else
#pragma warning(disable:4100)
		ProfilerSample*					Profiler::GetTop() const { return nullptr; }
		const	ProfilerSample*			Profiler::GetPreviousFrame( int skipCount ) const{ return nullptr; }
		ProfilerSample*					Profiler::GetPreviousFrame( int skipCount ) { return nullptr; }
		bool							Profiler::IsPaused() const { return false; }
		bool							Profiler::IsProfilerOpen() const { return false; }
		
		std::vector<ProfilerSample*>	Profiler::GetOrderedPreviousFrames(){ return {}; }
		void							Profiler::UpdateReportBoxText(){}
		void							Profiler::UpdateStatusBoxText( float deltaSeconds ) {}
		void							Profiler::UpdateControlsBoxText() {}
		void							Profiler::UpdateGraphBoxValues() {}
		void							Profiler::PrintLastFrameToConsole( ReportType type ){}
		void							Profiler::ClearReports() {}
		void							Profiler::ProcessInput(){}
		void							Profiler::ProcessMouseInput() {}
		void							Profiler::PushProfile( char id[] ) {}
		void							Profiler::PopProfile() {}
		ProfilerSample*					Profiler::CreateSample( char id[] ) { return nullptr; }
		void							Profiler::StartProfileFrame() {}
		void							Profiler::PauseProfiler() {}
		void							Profiler::ResumeProfiler() {}
		void							Profiler::Update( float deltaSeconds ) {}
		void							Profiler::Render() const {}
		Profiler*						Profiler::CreateInstance() { return nullptr; }
		void							Profiler::DestroyInstance() {}
		Profiler*						Profiler::GetInstance() {  return nullptr; }
		void							Profiler::Push( char id[] ) {}
		void							Profiler::Pop() {}
		void							Profiler::MarkFrame() {}
		bool							Profiler::PauseProfilerCommand( Command& cmd ) { return false; }
		bool							Profiler::ResumeProfilerCommand( Command& cmd ) { return false; }
		void							Profiler::CPUGraphClickListener( int selectedIndex, MouseButton buttonCode ) {}

		void							ProfilerStartup() {}
		void							ProfilerShutdown() {}

#endif // ENABLE_ENGINE_PROFILING