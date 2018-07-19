#include "Engine/UI/Widgets/Widget.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Window.hpp"

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
Widget::Widget(const std::string& newName, Canvas* canvas)
{
	name = newName;
	canvasRef = canvas; 
	transform = new Transform();
}

//-----------------------------------------------------------------------------------------------
// Sets the bounds of the widget
//
void Widget::SetBounds(const Vector2& mins, const Vector2& maxs)
{
	SetBounds(AABB2(mins, maxs));
}

//-----------------------------------------------------------------------------------------------
// Sets the bounds of the widget
//
void Widget::SetBounds(const AABB2& newBounds)
{
	bounds = newBounds;
}

//-----------------------------------------------------------------------------------------------
// Returns true if the mouse is inside the widget
//
bool Widget::IsMouseInWidget() const
{
	InputSystem* input = InputSystem::GetInstance();
	Window*	window = Window::GetInstance();
	
	Vector2	mousePos = input->GetMousePosition();
	AABB2 screenBounds;
	screenBounds.mins = RangeMap(bounds.mins, Vector2(0.f), Vector2(1.f), Vector2::ZERO, Vector2(window->m_width, window->m_height));
	screenBounds.maxs = RangeMap(bounds.maxs, Vector2(0.f), Vector2(1.f), Vector2::ZERO, Vector2(window->m_width, window->m_height));

	if(screenBounds.IsPointInside(mousePos))
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
// Processes mouse input
//
void Widget::ProcessMouseInput()
{
	if(!HasFocus())
	{
		return;
	}
}

//-----------------------------------------------------------------------------------------------
// Processes key input 
//
void Widget::ProcessInput()
{

}
