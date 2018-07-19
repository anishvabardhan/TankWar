#include "Engine/Core/Rgba.hpp"
#include <string>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <string.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector4.hpp"

//-----------------------------------------------------------------------------------------------
// Color constants
STATIC const Rgba Rgba::WHITE = Rgba(255,255,255,255);
STATIC const Rgba Rgba::RED = Rgba(255,0,0,255);
STATIC const Rgba Rgba::BLUE = Rgba(0,0,255,255);
STATIC const Rgba Rgba::GREEN = Rgba(0,255,0,255);
STATIC const Rgba Rgba::CYAN = Rgba(255,0,255,255);
STATIC const Rgba Rgba::MAJENTA = Rgba(255,255,0,255);
STATIC const Rgba Rgba::YELLOW = Rgba(255,255,0,255);

//-----------------------------------------------------------------------------------------------
// Operator ==
//
bool Rgba::operator==(const Rgba& compare) const
{
	if(r == compare.r &&
		g == compare.g &&
		b == compare.b)
	{
		return true;
	}
	return false;
}

const Rgba Rgba::BLACK = Rgba(0,0,0,1);

//-----------------------------------------------------------------------------------------------
// Constructor
//
Rgba::Rgba()
{
	r = 255;
	g = 255;
	b = 255;
	a = 255;
}

//-----------------------------------------------------------------------------------------------
// Constructor
//
Rgba::Rgba(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

//-----------------------------------------------------------------------------------------------
// Constructor
//
Rgba::Rgba(const Vector4& floats)
{
	SetAsFloats(floats.r, floats.g, floats.b, floats.a);
}

//-----------------------------------------------------------------------------------------------
// Returns color as floats through the arguments
//
void Rgba::GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const
{
	out_normalizedRed = r / 255.f;
	out_normalizedGreen = g / 255.f;
	out_normalizedBlue = b / 255.f;
	out_normalizedAlpha = a / 255.f;
}

//-----------------------------------------------------------------------------------------------
// Returns the color as floats through an array
//
float* Rgba::GetAsFloats() const
{
	float* data = new float[4] {
		(float) r / 255.f,
		(float) g / 255.f,
		(float) b / 255.f,
		(float) a / 255.f
	};

	return data;
}

//-----------------------------------------------------------------------------------------------
// Returns the color as a vector4
//
Vector4 Rgba::GetAsVector() const
{
	Vector4 color;
	
	color.x = (float) r * 1.f / 255.f;
	color.y = (float) g * 1.f / 255.f;
	color.z = (float) b * 1.f / 255.f;
	color.w = (float) a * 1.f / 255.f;

	return color;
}

//-----------------------------------------------------------------------------------------------
// Set the RGBA values as bytes(0-255)
//
void Rgba::SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

//-----------------------------------------------------------------------------------------------
// Sets RGBA values as floats(0.f-1.f)
//
void Rgba::SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha)
{
	r = (unsigned char) (normalizedRed * 255.f);
	if(r>255)
		r = 255;

	g = (unsigned char) (normalizedGreen * 255.f);
	if(g>255)
		g = 255;

	b = (unsigned char) (normalizedBlue * 255.f);
	if(b>255)
		b = 255;

	a = (unsigned char) (normalizedAlpha * 255.f);
	if(a>255)
		a = 255;
}

//-----------------------------------------------------------------------------------------------
// Scales the RGB values 
//
void Rgba::ScaleRGB(float rgbScale)
{
	float temp = (float) r * rgbScale;
	if(temp >255)
		r = 255;
	else
		r = (unsigned char) temp;

	temp = (float) g *  rgbScale;
	if(temp >255)
		g = 255;
	else
		g = (unsigned char)temp;

	temp = (float)b * rgbScale;
	if(b >255)
		b = 255;
	else
		b = (unsigned char) temp;

}

//-----------------------------------------------------------------------------------------------
// Scales the alpha value
//
void Rgba::ScaleAlpha(float alphaScale)
{
	float temp = (float) a * alphaScale;
	if(a>255)
		a = 255;
	else
		a = (unsigned char) temp;

}

//-----------------------------------------------------------------------------------------------
// Sets the value from text
//
void Rgba::SetFromText(const char* text)
{
	std::string rgba = text;
	std::string newA, newR, newG, newB;
	size_t commaIndex = 0, prevComma = 0;
	size_t countComma = 0;
	
	// Find number of commas
	for(unsigned int index=0; index<rgba.length(); ++index)
	{
		if(rgba[index] == ',')
			countComma++;
	}

	//Tokenize the string for R
	commaIndex = rgba.find(",", commaIndex);
	newR = std::string(rgba, prevComma, commaIndex);
	prevComma = commaIndex ;

	//Tokenize the string for G
	commaIndex = rgba.find(",", commaIndex+1);
	newG = std::string(rgba, prevComma+1, commaIndex);
	prevComma = commaIndex;
	
	//Tokenize the string for B
	commaIndex = rgba.find(",", commaIndex+1);
	newB = std::string(rgba, prevComma+1, commaIndex);
	prevComma = commaIndex;
	
	//Tokenize the string for A
	newA = std::string(rgba, prevComma+1);
	
	
	GUARANTEE_OR_DIE(!newR.empty(), "Rgba value requires R");
	GUARANTEE_OR_DIE(!newG.empty(), "Rgba value requires G");
	GUARANTEE_OR_DIE(!newB.empty(), "Rgba value requires B");
	
	r = (unsigned char) atoi(newR.c_str());
	g = (unsigned char) atoi(newG.c_str());
	b = (unsigned char) atoi(newB.c_str());
	if(countComma == 2)
		a = (unsigned char) 255;
	else
		a = (unsigned char) atoi(newA.c_str());
	//DebuggerPrintf("Red: %d Green:%d Blue:%d Alpha:%d", r,g,b,a);
}

//-----------------------------------------------------------------------------------------------
// Interpolates the color over time
//
Rgba Interpolate(const Rgba& startColor, const Rgba& endColor, float fractionTowardEnd)
{
	Rgba temp;
	temp.r = Interpolate(startColor.r, endColor.r, fractionTowardEnd);
	temp.g = Interpolate(startColor.g, endColor.g, fractionTowardEnd);
	temp.b = Interpolate(startColor.b, endColor.b, fractionTowardEnd);
	temp.a = Interpolate(startColor.a, endColor.a, fractionTowardEnd);

	return temp;
}
