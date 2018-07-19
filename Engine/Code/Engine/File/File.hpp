#include <stdio.h>
#include "Engine/Enumerations/FileMode.hpp"

//-----------------------------------------------------------------------------------------------
// Forward Declarations


//-----------------------------------------------------------------------------------------------
class File
{
public:
	//-----------------------------------------------------------------------------------------------
	// Constructors/Destructors
	File();
	~File();
	
	//-----------------------------------------------------------------------------------------------
	// Accessors/Mutators
	
	//-----------------------------------------------------------------------------------------------
	// Methods
	int		Open( const char* fileName, FileMode mode );
	void	Close();
	void	Flush();
	void	Printv( const char* format, va_list args );
	void	Printf( const char* format, ... );
	
	//-----------------------------------------------------------------------------------------------
	// Members
private:
	FILE*	fp = nullptr;
};

//-----------------------------------------------------------------------------------------------
// Standalone functions

// Reads the file into a buffer
void* FileReadToNewBuffer( char const *filename, size_t *out_size = nullptr );

// Writes a buffer into a file
bool FileWriteToNewFile( const char* filename, const char* data, size_t length );

// Appends a buffer into a file
bool FileAppendToFile( const char* fileName, const char* data, size_t length );

// Write to a png file
bool WriteToPng( const char* filename, const unsigned char* data, int width, int height, int numComponents );