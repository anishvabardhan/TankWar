#pragma once
#include "Engine/UI/Widgets/Widget.hpp"
#include <vector>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Canvas;
class Vector2;
enum MouseButton;
class StopWatch;

//-----------------------------------------------------------------------------------------------
// Function pointer
typedef void (*GraphClickCb)(int, MouseButton);

//-----------------------------------------------------------------------------------------------
class Widget_AreaGraph : public Widget
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	Widget_AreaGraph( const std::string& name, Canvas* canvas );
	~Widget_AreaGraph();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
			std::vector<float>	GetValues() const { return values; }
			void				SetValues( const std::vector<float>& newValues ) { values = newValues; }
			void				SetOnClick( GraphClickCb cb );
			AABB2				GetDrawBounds() const;
			float				GetStepSize() const;
			float				GetXBoundedValue( float value ) const;
			float				GetYBoundedValue( float value ) const;
			AABB2				GetSelectionBounds() const;
			void				SetThresholds( float goodValue, float cautionValue );

	//-----------------------------------------------------------------------------------------------
	// Methods
	virtual void				ProcessInput() override;
	virtual void				ProcessMouseInput() override;
	virtual void				Update( float deltaSeconds ) override;
	virtual void				Render() const override;
			void				HandleMouseLMB( const Vector2& mousePos );
			void				HandleMouseRMB( const Vector2& mousePos );
			void				AdjustMaxValue();
			Rgba				GetColorForValue( float value );
	
	//-----------------------------------------------------------------------------------------------
	// Members
	std::vector<float>		values;
	Rgba					bgColor;
	Rgba					goodColor;
	Rgba					cautionColor;
	Rgba					badColor;
	Mesh*					graphMesh;
	float					maxValue = 10.f;
	float					localMaxValue = 10.f;
	GraphClickCb			OnClickCb;
	int						selectedIndex = -1;
	StopWatch*				maxAdjustInterval;
	float					maxGoodValue = INFINITY;
	float					maxCautionValue = INFINITY;
};

