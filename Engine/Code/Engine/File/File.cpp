#include "Engine/File/File.hpp"
#include <stdlib.h>
#include <string>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "ThirdParty/stb/stb_image_write.h"

//-----------------------------------------------------------------------------------------------
// Constructor
//
File::File()
{

}

//-----------------------------------------------------------------------------------------------
// Destructor
//
File::~File()
{
	if(fp != nullptr)
	{
		fclose(fp);
	}
}

//-----------------------------------------------------------------------------------------------
// Tries to open a file and returns true if successful
//
int File::Open(const char* fileName, FileMode mode)
{
	int errorCode = fopen_s( &fp, fileName, GetFileModeString(mode) );

	return errorCode;
}

//-----------------------------------------------------------------------------------------------
// Closes the file and returns true if successful
//
void File::Close()
{
	fclose(fp);
}

//-----------------------------------------------------------------------------------------------
// Any unwritten data in the output buffer is written to the file
//
void File::Flush()
{
	fflush(fp);
}

//-----------------------------------------------------------------------------------------------
// Appends data to the file 
//
void File::Printv(const char* format, va_list args)
{
	const int MAX_LENGTH = 2048;
	char buffer[MAX_LENGTH];

	vsnprintf_s(buffer, MAX_LENGTH, _TRUNCATE, format, args);
	buffer[MAX_LENGTH - 1] = '\0';

	size_t length = strlen(buffer);
	fwrite(buffer, sizeof(char), length, fp);
}

//-----------------------------------------------------------------------------------------------
// Appends data to the file
//
void File::Printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Printv(format, args);
	va_end(args);
}

//-----------------------------------------------------------------------------------------------
// Reads the file into a buffer
//
void* FileReadToNewBuffer(char const *filename, size_t *out_size /*=nullptr */)
{

	FILE *fp = nullptr;
	fopen_s( &fp, filename, "r" );

	if (fp == nullptr) {
		return nullptr;
	}

	size_t size = 0U;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	unsigned char *buffer = (unsigned char*) malloc(size + 1U); // space for NULL

	size_t read = fread( buffer, 1, size, fp );
	fclose(fp);

	buffer[read] = NULL;

	// Returns size if the argument is passed
	if(out_size)
	{
		*out_size = size;
	}
	return buffer; 

}

//-----------------------------------------------------------------------------------------------
// Writes data into a file
//
bool FileWriteToNewFile(const char* fileName, const char* bufferData, size_t count)
{
	FILE *fp = nullptr;
	fopen_s( &fp, fileName, "w" );

	if (fp == nullptr) {
		return false;
	}

	fseek(fp, 0L, SEEK_SET);

	fwrite(bufferData, sizeof(char), count, fp);
	fclose(fp);

	return true;
}

//-----------------------------------------------------------------------------------------------
// Appends data to the end of the file
//
bool FileAppendToFile(const char* fileName, const char* data, size_t length)
{
	FILE* fp = nullptr;
	fopen_s(&fp, fileName, "a");

	if(fp == nullptr)
	{
		return false;
	}

	fwrite(data, sizeof(char), length, fp);
	fclose(fp);

	return true;
}

//-----------------------------------------------------------------------------------------------
// Writes data into a png
//
bool WriteToPng(const char* filename, const unsigned char* data, int width, int height, int numComponents)
{
	stbi_flip_vertically_on_write(true);
	if(stbi_write_png(filename, width, height, numComponents, data, 0) == 0)
		return false;
	stbi_flip_vertically_on_write(false);
	return true;
}