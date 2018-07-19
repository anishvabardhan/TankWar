#pragma once
#include "Engine/Profiler/ProfilerSample.hpp"
#include "Engine/Profiler/ProfileLogScope.hpp"
#include "Engine/Enumerations/ReportType.hpp"
#include "Engine/Enumerations/ReportSortMode.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Command;
class ProfilerReport;
class Canvas;
class Widget_Textbox;
class Widget_AreaGraph;
enum MouseButton;

//-----------------------------------------------------------------------------------------------
class Profiler
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	Profiler();
	~Profiler();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
			ProfilerSample*					GetTop() const;
	const	ProfilerSample*					GetPreviousFrame( int skipCount = 0 ) const;
			ProfilerSample*					GetPreviousFrame( int skipCount = 0 );
			bool							IsPaused() const;
			bool							IsProfilerOpen() const;

	//-----------------------------------------------------------------------------------------------
	// Methods
			void							PushProfile( char id[] );
			void							PopProfile();
			ProfilerSample*					CreateSample( char id[] );
			void							StartProfileFrame();
			void							PauseProfiler();
			void							ResumeProfiler();
			std::vector<ProfilerSample*>	GetOrderedPreviousFrames();
			void							Update( float deltaSeconds );
			void							UpdateReportBoxText();
			void							UpdateStatusBoxText();
			void							UpdateControlsBoxText();
			void							UpdateGraphBoxValues();
			void							Render() const;
			void							PrintLastFrameToConsole( ReportType type );
			void							ClearReports();
			void							ProcessInput();
			void							ProcessMouseInput();

	//-----------------------------------------------------------------------------------------------
	// Static Methods
	static	Profiler*						CreateInstance();
	static	void							DestroyInstance();
	static	Profiler*						GetInstance();
	static	void							Push( char id[] );
	static	void							Pop();
	static	void							MarkFrame();
	static	void							Close();
	static	void							Open(); 
	
	//-----------------------------------------------------------------------------------------------
	// Click Listeners
	static	void							CPUGraphClickListener( int selectedIndex, MouseButton buttonCode );

	//-----------------------------------------------------------------------------------------------
	// Command Callbacks
	static	bool							PauseProfilerCommand( Command& cmd );
	static	bool							ResumeProfilerCommand( Command& cmd );
	static	bool							ProfilerCommand( Command& cmd );
	static	bool							ProfilerReportCommand( Command& cmd );

	//-----------------------------------------------------------------------------------------------
	// Members
			ProfilerSample*					m_activeNode = nullptr;
			std::vector<ProfilerSample*>	m_prevStacks;
			unsigned int					m_prevFrameIndex = 0;
			bool							m_isReadyToPause = false;
			bool							m_isReadyToResume = false;
			bool							m_isPaused = false;
			bool							m_isProfilerOpen = false;
			Canvas*							m_canvas;
			std::vector<ProfilerReport*>	m_reports;
			Widget_Textbox*					m_statusBoxRef;
			Widget_Textbox*					m_controlsBoxRef;
			Widget_Textbox*					m_reportBoxRef;
			Widget_AreaGraph*				m_graphBoxRef;
			ReportType						m_reportViewType = REPORT_TYPE_TREE;
			bool							m_hasMouseControl = false;
			ReportSortMode					m_reportSortMode = SORT_BY_NONE;
			int								m_selectedFrame = 0;
};

//-----------------------------------------------------------------------------------------------
// Standalone functions
void	ProfilerStartup();
void	ProfilerShutdown();

//-----------------------------------------------------------------------------------------------
// PROFILER MACROS
#define PROFILE_LOG_SCOPE(tag)				ProfileLogScope __timer_ ##__LINE__ ## (tag);
#define PROFILE_LOG_SCOPE_FUNCTION()		PROFILE_LOG_SCOPE(__FUNCTION__);