#include "Engine/UI/Widgets/Widget_AreaGraph.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/UI/Canvas.hpp"
#include "Engine/Renderer/Mesh/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh/Mesh.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/StopWatch.hpp"
#include "Engine/Core/EngineCommon.hpp"
//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
// Constructor
//
Widget_AreaGraph::Widget_AreaGraph(const std::string& name, Canvas* canvas)
	: Widget(name, canvas)
{
	bgColor = Rgba(100,100,100,100);
	goodColor = Rgba(30, 255, 20, 100);
	cautionColor = Rgba(255, 195, 0, 100);
	badColor = Rgba(255, 0, 0, 100);

	maxAdjustInterval = new StopWatch();
	maxAdjustInterval->SetTimer(5.f);
}

//-----------------------------------------------------------------------------------------------
// Destructor
//
Widget_AreaGraph::~Widget_AreaGraph()
{

}

//-----------------------------------------------------------------------------------------------
// Sets the callback for on click events
//
void Widget_AreaGraph::SetOnClick(GraphClickCb cb)
{
	OnClickCb = cb;
}

//-----------------------------------------------------------------------------------------------
// Returns the draw bounds within the canvas
//
AABB2 Widget_AreaGraph::GetDrawBounds() const
{
	AABB2 drawBounds;
	drawBounds.mins = RangeMap(bounds.mins, Vector2::ZERO, Vector2::ONE, canvasRef->m_bounds.mins, canvasRef->m_bounds.maxs);
	drawBounds.maxs = RangeMap(bounds.maxs, Vector2::ZERO, Vector2::ONE, canvasRef->m_bounds.mins, canvasRef->m_bounds.maxs);

	return drawBounds;
}

//-----------------------------------------------------------------------------------------------
// Calculates stepsize for on x direction for the graph
//
float Widget_AreaGraph::GetStepSize() const
{
	if(values.size() == 0)
	{
		return 0;
	}

	AABB2 drawBounds = GetDrawBounds();
	float length = drawBounds.maxs.x - drawBounds.mins.x;

	float stepSize = length / (float) values.size();

	return stepSize;
}

//-----------------------------------------------------------------------------------------------
// Returns the value bounded by the x-bounds of the graph widget
//
float Widget_AreaGraph::GetXBoundedValue(float value) const
{
	AABB2 drawBounds = GetDrawBounds();
	float xMax = drawBounds.maxs.x - drawBounds.mins.x;
	float bounded  = RangeMapFloat(value, 0.f, 1.f, 0.f, xMax);
	
	return bounded;
}

//-----------------------------------------------------------------------------------------------
// Returns the value bounded by the y-bounds of the graph widget
//
float Widget_AreaGraph::GetYBoundedValue( float value ) const
{
	AABB2 drawBounds = GetDrawBounds();
	float yMax = drawBounds.maxs.y - drawBounds.mins.y;
	float bounded  = RangeMapFloat(value, 0.f, maxValue, 0.f, yMax);

	return bounded;
}

//-----------------------------------------------------------------------------------------------
// Returns the selected bounds
//
AABB2 Widget_AreaGraph::GetSelectionBounds() const
{
	AABB2 selectBounds;
	
	float stepSize = GetStepSize();
	selectBounds.mins.x = (selectedIndex * stepSize) - 0.001f;
	selectBounds.mins.y = 0.f;
	selectBounds.maxs.x = (selectedIndex * stepSize) + 0.001f;
	selectBounds.maxs.y = maxValue;

	selectBounds.mins.x = bounds.mins.x + selectBounds.mins.x;
	selectBounds.mins.y = bounds.mins.y + GetYBoundedValue(selectBounds.mins.y);
	selectBounds.maxs.x = bounds.mins.x + selectBounds.maxs.x;
	selectBounds.maxs.y = bounds.mins.y + GetYBoundedValue(selectBounds.maxs.y);

	return selectBounds;
}

//-----------------------------------------------------------------------------------------------
// Set the threshold values for color coding
//
void Widget_AreaGraph::SetThresholds(float good, float caution)
{
	maxGoodValue = good;
	maxCautionValue = caution;
}

//-----------------------------------------------------------------------------------------------
// Processes key input when it has focus
//
void Widget_AreaGraph::ProcessInput()
{

}

//-----------------------------------------------------------------------------------------------
// Processes mouse input when in focus
//
void Widget_AreaGraph::ProcessMouseInput()
{
	if(!HasFocus())
	{
		return;
	}

	InputSystem* input = InputSystem::GetInstance();

	if(input->WasMouseButtonPressed(MOUSE_LMB))
	{
		Vector2 mousePos = input->GetMousePosition();
		HandleMouseLMB(mousePos);
	}

	if(input->WasMouseButtonPressed(MOUSE_RMB))
	{
		Vector2 mousePos = input->GetMousePosition();
		HandleMouseRMB(mousePos);
	}
}

//-----------------------------------------------------------------------------------------------
// Updates the area graph
//
void Widget_AreaGraph::Update(float deltaSeconds)
{
	ProcessMouseInput();
	ProcessInput();

	if(maxAdjustInterval->DecrementAll())
	{
		AdjustMaxValue();
	}

	if(values[values.size() - 1] > maxValue)
	{
		maxValue = values[values.size() - 1];
	}

	AABB2 drawBounds = GetDrawBounds();
	float stepSize = GetStepSize();
	Vector2 drawMins = drawBounds.mins;

	MeshBuilder builder;
	builder.Begin(PRIMITIVE_TRIANGLES, true);
	
	localMaxValue = 0.f;
	Rgba color1;
	Rgba color2;
	float curVal;
	float nextVal;
	for(int index = 0; index < (int) values.size() - 1; ++index)
	{
		curVal = values[index];
		nextVal = values[index];

		if(curVal > localMaxValue)
		{
			localMaxValue = curVal; 
		}

		color1 = GetColorForValue(curVal);
		color2 = GetColorForValue(nextVal);

		builder.SetColor(color1);
		builder.SetUV(0.f, 0.f);
		int idx = builder.PushVertex(drawMins + Vector2(stepSize * index, 0.f));

		builder.SetColor(color2);
		builder.SetUV(1.f, 0.f);
		builder.PushVertex(drawMins + Vector2(stepSize * (index + 1), 0.f));

		builder.SetColor(color2);
		builder.SetUV(1.f, 1.f);
		builder.PushVertex(drawMins + Vector2(stepSize * (index + 1), GetYBoundedValue(values[index + 1])));

		builder.SetColor(color1);
		builder.SetUV(0.f, 1.f);
		builder.PushVertex(drawMins + Vector2(stepSize * index, GetYBoundedValue(values[index])));

		builder.AddQuadIndices(idx, idx+1, idx+2, idx+3);
	}

	builder.End();

	if(graphMesh)
	{
		delete graphMesh;
	}

	graphMesh = builder.CreateMesh<Vertex_3DPCU>();

	if(HasFocus())
	{
		bgColor.a = 255;
	}
	else
	{
		bgColor.a = 100;
	}

	UNUSED(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
// Renders the area graph
//
void Widget_AreaGraph::Render() const
{
	Renderer* rend = Renderer::GetInstance();
	rend->SetDefaultTexture();
	rend->DrawAABB2(GetDrawBounds(), bgColor);
	
	rend->DrawMesh(graphMesh);

	rend->SetDefaultTexture();
	if(selectedIndex > -1)
	{
		rend->DrawAABB2(GetSelectionBounds(), Rgba::BLUE);
	}
}

//-----------------------------------------------------------------------------------------------
// Handles left mouse button click. Selects the sample and draws a line.
//
void Widget_AreaGraph::HandleMouseLMB(const Vector2& mousePos)
{
	// Assumes mouse is inside the widget (ie Widget in focus)

	Window* window = Window::GetInstance();
	int	numValues = (int) values.size();

	float boundedX = RangeMapFloat(mousePos.x, 0.f, window->m_width, 0.f, 1.f);
	boundedX -= bounds.mins.x;

	float sampleUnderMouse = boundedX * 1.f / GetStepSize();
	
	int sampleIndex = (int) floor(sampleUnderMouse);

	selectedIndex = sampleIndex;

	if(OnClickCb)
	{
		OnClickCb(selectedIndex, MOUSE_LMB);
	}
}

//-----------------------------------------------------------------------------------------------
// Handles right mouse button click. De-selects the sample (if any)
//
void Widget_AreaGraph::HandleMouseRMB(const Vector2& mousePos)
{
	// Assumes mouse is inside the widget (ie Widget in focus)
	selectedIndex = -1;

	if(OnClickCb)
	{
		OnClickCb(selectedIndex, MOUSE_RMB);
	}
}

//-----------------------------------------------------------------------------------------------
// Adjusts max value over time to the highest of the values
//
void Widget_AreaGraph::AdjustMaxValue()
{
	maxValue = localMaxValue;
}

//-----------------------------------------------------------------------------------------------
// Checks the value against the three thresholds and returns the corresponding color
//
Rgba Widget_AreaGraph::GetColorForValue(float value)
{
	if(value <= maxGoodValue)
	{
		return goodColor;
	}
	else if(value <= maxCautionValue)
	{
		return cautionColor;
	}
	else
	{
		return badColor;
	}
}
