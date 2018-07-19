#include "Engine/Core/Image.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"

Image* Image::s_defaultTexture = nullptr;

//-----------------------------------------------------------------------------------------------
// Constructor
//
Image::Image(const std::string& imageFilePath, bool flipY)
{
	int numComponents = 0;
	int numComponentsReq = 0;
	stbi_set_flip_vertically_on_load(flipY);
	unsigned char* imageData = stbi_load(imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsReq);
	PopulateDataFromImage(imageData, numComponents);
	stbi_set_flip_vertically_on_load(false);
	stbi_image_free(imageData);
	m_imagePath = imageFilePath;
}

//-----------------------------------------------------------------------------------------------
// Constructor
//
Image::Image(const Rgba& singlePixelColor, const std::string& name)
{
	m_imagePath = name;
	m_texels.push_back(singlePixelColor);
	m_dimensions = IntVector2(1,1);
}

//-----------------------------------------------------------------------------------------------
// Populates the texel vector with data from the image
// 
void Image::PopulateDataFromImage( unsigned char* imageData, int numComponents )
{
	if(numComponents == 3)
	{
		long long numValues = m_dimensions.x * m_dimensions.y * numComponents;
		for(long long index = 0; index < numValues; index += numComponents)
		{
			Rgba texel;
			texel.r = imageData[index];
			texel.g = imageData[index+1];
			texel.b = imageData[index+2];
			texel.a = (unsigned char) 255;
			m_texels.push_back(texel);
		}
	}

	if(numComponents == 4)
	{
		long long numValues = m_dimensions.x * m_dimensions.y * numComponents;
		for(long long index = 0; index < numValues; index += numComponents)
		{
			Rgba texel;
			texel.r = imageData[index];
			texel.g = imageData[index+1];
			texel.b = imageData[index+2];
			texel.a = imageData[index+3];
			m_texels.push_back(texel);
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Generates the default textures
//
void Image::GenerateDefaultTextures()
{
	s_defaultTexture = new Image(Rgba::WHITE, "default");
}

//-----------------------------------------------------------------------------------------------
// Return the texel's color as Rgba
//
Rgba Image::GetTexel(int x, int y) const
{
	if(x >= 0 && x < m_dimensions.x && y >=0 && y < m_dimensions.y)
	{
		int index;
		index = (y * m_dimensions.x) + x;
		return m_texels[index];
	}

	else
	{
		return Rgba(0,0,0,0);
	}
}

//-----------------------------------------------------------------------------------------------
// Sets the value of the texel 
//
void Image::SetTexel(int x, int y, const Rgba& color)
{
	if(x >= 0 && x < m_dimensions.x && y >=0 && y < m_dimensions.y)
	{
		int index;
		index = (y * m_dimensions.x) + x;
		m_texels[index] = color;
	}

	else
	{
		return;
	}
}

//-----------------------------------------------------------------------------------------------
// Gets the reference to the texel of coordinates
//
Rgba* Image::GetTexelReference(int x, int y)
{
	if(x >= 0 && x < m_dimensions.x && y >= 0 && y < m_dimensions.y)
	{
		int index;
		index = (y * m_dimensions.x) + x;
		return &m_texels[index];
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
// Returns a const reference to the texel at coordinates
//
const Rgba* Image::GetTexelReference(int x, int y) const
{
	if(x >= 0 && x < m_dimensions.x && y >= 0 && y < m_dimensions.y)
	{
		int index;
		index = (y * m_dimensions.x) + x;
		return &m_texels[index];
	}

	return nullptr;
}

//-----------------------------------------------------------------------------------------------
// Returns the image texels as a byte array
//
unsigned char* Image::GetTexelsAsByteArray() const
{
	return (unsigned char*) m_texels.data();
}
