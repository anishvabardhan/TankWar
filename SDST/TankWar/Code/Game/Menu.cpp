#include "Game/Menu.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Console/DevConsole.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Profiler/Profiler.hpp"

const unsigned int INVALID_INDEX = 0xffffffffU;

//-----------------------------------------------------------------------------------------------
// Constructor
//
Menu::Menu()
{
	m_currentIndex = 0;
	m_camera = new Camera();
	m_camera->SetColorTarget(Renderer::GetInstance()->m_defaultColorTarget);
	m_shader = Shader::AcquireResource("Data/Shaders/text.shader");
}

//-----------------------------------------------------------------------------------------------
// Returns the selected index for this frame
//
unsigned int Menu::GetFrameSelection() const
{
	return m_selectedIndex;
}

//-----------------------------------------------------------------------------------------------
// Returns the immediate next active option
//
unsigned int Menu::GetNextActiveIndex() const
{
	unsigned int index = m_currentIndex;
	index++;

	if(index > (m_options.size() - 1))
	{
		index = 0;
	}
	return index;
}

//-----------------------------------------------------------------------------------------------
// Returns the immediate previous active option
//
unsigned int Menu::GetPrevActiveIndex() const
{
	unsigned int index = m_currentIndex;
	index--;

	if(index < 0)
	{
		index = (unsigned int) m_options.size() - 1;
		
	}
	return index;
}

//-----------------------------------------------------------------------------------------------
// Processes the input every frame 
//
void Menu::ProcessInput()
{
	m_selectedIndex = INVALID_INDEX;

	HandleKeyboardInput();

	m_currentIndex = ClampInt(m_currentIndex, 0, (int) m_options.size() - 1);
}

//-----------------------------------------------------------------------------------------------
// Handles the keyboard input
//
void Menu::HandleKeyboardInput()
{
	InputSystem* g_TheInput = InputSystem::GetInstance();

	if(IsDevConsoleOpen())
		return;

	if(g_TheInput->WasKeyJustPressed(KEYCODE_UP) || g_TheInput->WasKeyJustPressed(KEYCODE_W))
	{
		m_currentIndex = GetPrevActiveIndex();
		AudioSystem::GetInstance()->PlayOneOffGroup("menu.move");
	}

	if(g_TheInput->WasKeyJustPressed(KEYCODE_DOWN) || g_TheInput->WasKeyJustPressed(KEYCODE_S))
	{
		AudioSystem::GetInstance()->PlayOneOffGroup("menu.move");
		m_currentIndex = GetNextActiveIndex();
	}

	if(g_TheInput->WasKeyJustPressed(KEYCODE_ENTER) && m_options[m_currentIndex].active)
	{
		AudioSystem::GetInstance()->PlayOneOffGroup("menu.select");
		m_selectedIndex = m_currentIndex;
	}

	if(g_TheInput->WasKeyJustPressed(KEYCODE_ESCAPE))
	{
		m_pressedEscape = true;
	}
}

//-----------------------------------------------------------------------------------------------
// Renders the menu
//
void Menu::Render() const
{
	Window* window = Window::GetInstance();
	float fontSize = 0.03f * window->m_height;
	AABB2 bounds = AABB2(m_center, 100.f, m_options.size() * 30.f);
	Renderer* rend = Renderer::GetInstance();
	rend->SetCamera(m_camera);
	rend->SetOrtho(Matrix44::MakeOrtho3D(0.f, window->m_width, 0.f, window->m_height, -1.f, 1.f));
	rend->SetDefaultTexture();
	rend->SetDefaultMaterial();
	rend->SetShader(m_shader);
	rend->DrawAABB2(bounds, Rgba(0,255,0,50));
	Vector2 drawMins = bounds.mins;
	for(std::vector<MenuOption>::const_reverse_iterator line = m_options.rbegin(); line != m_options.rend(); ++line)
	 {
		MenuOption option = *line;
		drawMins.y += fontSize;
		bounds.mins.y += fontSize * 1.2f;
		bounds.maxs.y += fontSize * 1.2f;
		Rgba color = Rgba::WHITE;
		if(!option.active)
		{
			if(option.value == m_currentIndex)
			{
				color = Rgba(100,100,100,255);
			}
			else
			{
				color = Rgba(50,50,50,255);
			}
			
		}

		else if(option.value == m_currentIndex)
		{
			color = Rgba::GREEN;
		}			
		rend->DrawTextInBox2D(bounds, option.text, fontSize, TextAlignment::ALIGN_BOTTOM_CENTER, OVERRUN, rend->CreateOrGetBitmapFont("SquirrelFixedFont"), color);
	 }
	rend->SetDepthTestMode(COMPARE_LESS, true);
}

//-----------------------------------------------------------------------------------------------
// Adds a new option to the menu
//
void Menu::AddOption(const char* text, unsigned int index)
{
	MenuOption option;
	option.text = text;
	option.value = index;
	option.active = true;

	m_options.push_back(option);
}

//-----------------------------------------------------------------------------------------------
// Adds all the options specified in the argument
//
void Menu::AddOptions(std::vector<MenuOption*> options)
{
	for( MenuOption* option : options)
	{
		m_options.push_back(*option);
	}
}

//-----------------------------------------------------------------------------------------------
// Disables an option (Greys it out)
//
void Menu::DisableOption(unsigned int index)
{
	m_options[index].active = false;
}
