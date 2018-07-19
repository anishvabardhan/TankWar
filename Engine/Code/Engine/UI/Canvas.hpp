#pragma once
#include <vector>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Widget;
class Camera;
class Widget_Textbox;
class Widget_AreaGraph;
class Vector2;
class Shader;

//-----------------------------------------------------------------------------------------------
class Canvas
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	Canvas();
	~Canvas();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	void				SetBounds( const Vector2& mins, const Vector2& maxs );
	void				SetBounds( const AABB2& newBounds );
	void				SetBGColor( const Rgba& color ) { m_bgColor = color; }
	bool				IsMouseInCanvas() const;
	void				SetMouseInput( bool flag );
	void				SetKeyInput( bool flag );

	//-----------------------------------------------------------------------------------------------
	// Methods
	void				ProcessMouseInput();
	void				ProcessInput();
	void				Update( float deltaSeconds );
	void				Render() const;

	//-----------------------------------------------------------------------------------------------
	// Widget Adders
	Widget_AreaGraph*	AddAreaGraph( const std::string& name, const Vector2& mins, const Vector2& maxs, const std::vector<float>& newValues );
	Widget_Textbox*		AddTextBox( const std::string& name, const Vector2& mins, const Vector2& maxs, const std::string& text );

	//-----------------------------------------------------------------------------------------------
	// Members
	Camera*					m_uiCamera;
	std::vector<Widget*>	m_widgets;
	AABB2					m_bounds = AABB2::ZERO_TO_ONE;
	Shader*					m_shader;
	Rgba					m_bgColor;
	bool					m_hasMouseControl = false;
	bool					m_hasKeyControl = false;
};

