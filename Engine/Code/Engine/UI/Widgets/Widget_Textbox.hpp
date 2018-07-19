#pragma once
#include "Engine/UI/Widgets/Widget.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Canvas;
class BitmapFont;

//-----------------------------------------------------------------------------------------------
class Widget_Textbox : public Widget
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	Widget_Textbox( const std::string& name, Canvas* canvas );
	~Widget_Textbox();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
			AABB2			GetDrawBounds() const;
			Vector2			GetMins() const { return bounds.mins; }
			Vector2			GetMaxs() const { return bounds.maxs; }
			void			SetText( const std::string& newText ) { text = newText; }
			void			SetTextColor( const Rgba& color ) { textColor = color; }
			std::string		GetText() const { return text; }
			void			SetFont( const BitmapFont* newFont );
			void			SetFont( const char* fontName );
			void			SetFontAspect( float aspect ) { fontAspect = aspect; }
			float			GetFontAspect() const { return fontAspect; }
	const	BitmapFont*		GetFont() const { return font; }
	
	//-----------------------------------------------------------------------------------------------
	// Methods
	virtual void			ProcessInput() override;
	virtual void			ProcessMouseInput() override;
	virtual void			Update( float deltaSeconds ) override;
	virtual void			Render() const override;

	//-----------------------------------------------------------------------------------------------
	// Members
			std::string		text = "";
			Rgba			textColor;
			Rgba			bgColor;
			float			fontSize = 0.011f;
			float			fontAspect = 1.f;
	const	BitmapFont*		font;
};

