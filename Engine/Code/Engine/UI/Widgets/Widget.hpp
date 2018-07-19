#pragma once
#include <string>
#include "Engine/Math/AABB2.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Transform;
class Mesh;
class Canvas;

//-----------------------------------------------------------------------------------------------
class Widget
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	Widget( const std::string& newName, Canvas* canvas );
	~Widget(){}
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
			std::string		GetName() const { return name; }
			Mesh*			GetMesh() const { return mesh; }
			AABB2			GetBounds() const { return bounds; }
			void			SetBounds( const Vector2& mins, const Vector2& maxs );
			void			SetBounds( const AABB2& newBounds );
			bool			IsMouseInWidget() const;
			bool			HasFocus() const { return hasFocus; }
			void			SetFocus( bool focus ) { hasFocus = focus; }

	//-----------------------------------------------------------------------------------------------
	// Methods
	virtual void			Update( float deltaSeconds ){ (void) deltaSeconds; }
	virtual void			Render() const = 0; // Pure virtual
	virtual	void			ProcessMouseInput();
	virtual void			ProcessInput();

	//-----------------------------------------------------------------------------------------------
	// Members
	Canvas*			canvasRef;
	std::string		name;
	Mesh*			mesh;
	Transform*		transform;
	AABB2			bounds;
	bool			hasFocus = false;
};

