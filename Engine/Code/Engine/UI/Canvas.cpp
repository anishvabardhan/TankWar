#include "Engine/UI/Canvas.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/UICamera.hpp"
#include "Engine/UI/Widgets/Widget.hpp"
#include "Engine/UI/Widgets/Widget_Textbox.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/UI/Widgets/Widget_AreaGraph.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Window.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
Canvas::Canvas()
{
	m_uiCamera = new UICamera();
	m_uiCamera->SetColorTarget(Renderer::GetInstance()->m_defaultColorTarget);
	m_uiCamera->SetDepthTarget(Renderer::GetInstance()->m_defaultDepthTarget);

	m_shader = new Shader(Renderer::GetInstance()->CreateOrGetShaderProgram("diffuse"));
	m_shader->SetColorBlending(BLEND_OP_ADD, BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
	m_shader->DisableDepth();

	m_bgColor = Rgba(0,0,255,50);
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
Canvas::~Canvas()
{
	for(Widget* widget : m_widgets)
	{
		delete widget;
		widget = nullptr;
	}

	delete m_uiCamera;
	m_uiCamera = nullptr;

	delete m_shader;
	m_shader = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Sets the canvas bounds
//
void Canvas::SetBounds(const Vector2& mins, const Vector2& maxs)
{
	SetBounds(AABB2(mins,maxs));
}

//-----------------------------------------------------------------------------------------------
// Sets the canvas bounds
//
void Canvas::SetBounds(const AABB2& newBounds)
{
	m_bounds = newBounds;
}

//-----------------------------------------------------------------------------------------------
// Returns true if the mouse is inside the canvas
//
bool Canvas::IsMouseInCanvas() const
{
	InputSystem* input = InputSystem::GetInstance();
	Window*	window = Window::GetInstance();

	Vector2	mousePos = input->GetMousePosition();
	AABB2 screenBounds;
	screenBounds.mins = RangeMap(m_bounds.mins, Vector2(0.f), Vector2(1.f), Vector2::ZERO, Vector2(window->m_width, window->m_height));
	screenBounds.maxs = RangeMap(m_bounds.maxs, Vector2(0.f), Vector2(1.f), Vector2::ZERO, Vector2(window->m_width, window->m_height));

	if(screenBounds.IsPointInside(mousePos))
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
// Sets the mouse input option on the canvas. "true" accepts input
//
void Canvas::SetMouseInput(bool flag)
{
	m_hasMouseControl = flag;
}

//-----------------------------------------------------------------------------------------------
// Sets the key input option on the canvas. "true" accepts input
//
void Canvas::SetKeyInput(bool flag)
{
	m_hasKeyControl = flag;
}

//-----------------------------------------------------------------------------------------------
// Processes mouse input if the canvas has focus 
//
void Canvas::ProcessMouseInput()
{
	if(!IsMouseInCanvas() && !m_hasMouseControl)
	{
		return;
	}

	for(Widget* widget : m_widgets)
	{
		widget->SetFocus(false);

		if(widget->IsMouseInWidget())
		{
			widget->SetFocus(true);
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Processes key input if the canvas has focus
//
void Canvas::ProcessInput()
{
// 	if(!IsMouseInCanvas() && !m_hasMouseControl)
// 	{
// 		return;
// 	}

// 	for(Widget* widget : m_widgets)
// 	{
// 		widget->SetFocus(false);
// 
// 		if(widget->IsMouseInWidget())
// 		{
// 			widget->SetFocus(true);
// 		}
// 	}
}

//-----------------------------------------------------------------------------------------------
// Adds a text box to the canvas
//
Widget_Textbox* Canvas::AddTextBox(const std::string& name, const Vector2& mins, const Vector2& maxs, const std::string& text)
{
	Widget_Textbox* textBox = new Widget_Textbox(name, this);
	textBox->SetBounds(mins, maxs);
	textBox->SetText(text);

	m_widgets.push_back(textBox);
	return textBox;
}

//-----------------------------------------------------------------------------------------------
// Updates the canvas
//
void Canvas::Update(float deltaSeconds)
{
	ProcessInput();
	ProcessMouseInput();

	for(Widget* widget : m_widgets)
	{
		widget->Update(deltaSeconds);
	}
}

//-----------------------------------------------------------------------------------------------
// Renders the widgets 
//
void Canvas::Render() const
{
	Renderer* rend = Renderer::GetInstance();
	rend->SetCamera(m_uiCamera);
	rend->SetDefaultTexture();
	rend->SetDefaultMaterial();
	rend->SetShader(m_shader);
	
	rend->DrawAABB2(m_bounds, m_bgColor);

	for(Widget* widget : m_widgets)
	{
		widget->Render();
	}
}

//-----------------------------------------------------------------------------------------------
// Adds an area graph widget to the canvas
//
Widget_AreaGraph* Canvas::AddAreaGraph(const std::string& name, const Vector2& mins, const Vector2& maxs, const std::vector<float>& newValues)
{
	Widget_AreaGraph* graphBox = new Widget_AreaGraph(name, this);
	graphBox->SetBounds(mins, maxs);
	graphBox->SetValues(newValues);

	m_widgets.push_back(graphBox);
	return graphBox;
}

