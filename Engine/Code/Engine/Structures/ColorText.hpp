#pragma once
#include <string>
#include "Engine/Core/Rgba.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
// To enable colored text in each line 
struct ColoredText
{
	ColoredText(){}
	ColoredText(const std::string str, const Rgba& col)
	{
		text = str;
		color = col;
	}

	std::string text;
	Rgba color;
};

