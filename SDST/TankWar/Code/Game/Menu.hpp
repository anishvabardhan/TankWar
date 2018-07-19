#pragma once
#include <string>
#include <vector>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations
extern const unsigned int INVALID_INDEX;
class Camera;
class Shader;

struct MenuOption
{
	MenuOption(){}
	MenuOption(const std::string& optionText, unsigned int optionValue, bool optionActive)
		: text(optionText), value(optionValue), active(optionActive)
	{}
	std::string		text; 
	int				value; 
	bool			active;
};

//-----------------------------------------------------------------------------------------------
class Menu
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	Menu();
	~Menu(){}
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	unsigned int	GetHoverIndex() const { return m_currentIndex; }
	unsigned int	GetFrameSelection() const;
	unsigned int	GetNextActiveIndex() const;
	unsigned int	GetPrevActiveIndex() const;
	std::string		GetOptionNameForIndex( int index ) const { return m_options[index].text; }
	std::string		GetFrameSelectedText() const { return GetOptionNameForIndex(m_selectedIndex); }
	std::string		GetHoverText() const { GetOptionNameForIndex(m_currentIndex); }
	bool			HasPressedEscape() const { return m_pressedEscape; }
	void			SetCenter(const Vector2& center) { m_center = center; }
	void			SetCurrentOption( int index ) { m_currentIndex = index; }

	//-----------------------------------------------------------------------------------------------
	// Methods
	void			ProcessInput();
	void			HandleKeyboardInput();
	void			Render() const;
	void			AddOption(const char* text, unsigned int index);
	void			AddOptions(std::vector<MenuOption*> options);
	void			DisableOption(unsigned int index);

	//-----------------------------------------------------------------------------------------------
	// Members
	int						m_currentIndex;
	int						m_selectedIndex;
	std::vector<MenuOption> m_options;
	AABB2					m_bounds;
	Vector2					m_center;
	Camera*					m_camera;
	bool					m_pressedEscape = false;
	Shader*					m_shader;
};

