#include "Engine/Enumerations/FileMode.hpp"
//-----------------------------------------------------------------------------------------------
// Engine Includes
#include "Engine/Core/ErrorWarningAssert.hpp"
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
// Standard Includes

//-----------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------
// Returns the file mode string to be used with fopen_s
//
char* GetFileModeString(FileMode mode)
{
	switch (mode)
	{
	case FILE_READ:		return "r"; break;
	case FILE_WRITE:	return "w"; break;
	case FILE_APPEND:	return "a"; break;
	default:
		GUARANTEE_OR_DIE(false, "Bad file mode selection");
		break;
	}
}
