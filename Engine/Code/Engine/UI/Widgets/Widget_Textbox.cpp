#include "Engine/UI/Widgets/Widget_Textbox.hpp"

//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/UI/Canvas.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Constructor
//
Widget_Textbox::Widget_Textbox(const std::string& name, Canvas* canvas)
	: Widget(name, canvas)
{
	bgColor = Rgba(200, 200, 200, 100);
	font = Renderer::GetInstance()->CreateOrGetBitmapFont("SquirrelFixedFont");
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
Widget_Textbox::~Widget_Textbox()
{

}

//-----------------------------------------------------------------------------------------------
// Returns the bounds within the canvas
//
AABB2 Widget_Textbox::GetDrawBounds() const
{
	AABB2 drawBounds;
	drawBounds.mins = RangeMap(bounds.mins, Vector2::ZERO, Vector2::ONE, canvasRef->m_bounds.mins, canvasRef->m_bounds.maxs);
	drawBounds.maxs = RangeMap(bounds.maxs, Vector2::ZERO, Vector2::ONE, canvasRef->m_bounds.mins, canvasRef->m_bounds.maxs);

	return drawBounds;
}

//-----------------------------------------------------------------------------------------------
// Sets the font for the textbox
//
void Widget_Textbox::SetFont(const char* fontName)
{
	SetFont(Renderer::GetInstance()->CreateOrGetBitmapFont(fontName));
}

//-----------------------------------------------------------------------------------------------
// Sets the font for the textbox
//
void Widget_Textbox::SetFont(const BitmapFont* newFont)
{
	font = newFont;
}

//-----------------------------------------------------------------------------------------------
// Processes key input
//
void Widget_Textbox::ProcessInput()
{

}

//-----------------------------------------------------------------------------------------------
// Processes mouse input
//
void Widget_Textbox::ProcessMouseInput()
{

}

//-----------------------------------------------------------------------------------------------
// Updates the text box
//
void Widget_Textbox::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Renders the widget
//
void Widget_Textbox::Render() const
{
	Renderer* rend = Renderer::GetInstance();
	rend->SetDefaultTexture();
	rend->DrawAABB2(GetDrawBounds(), bgColor);

	rend->DrawTextInBox2D(GetDrawBounds(), text, fontSize, TextAlignment::ALIGN_BOTTOM_LEFT, OVERRUN, font, textColor, fontAspect);
}
