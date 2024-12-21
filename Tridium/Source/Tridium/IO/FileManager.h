#pragma once
#include "FilePath.h"

namespace Tridium
{
	namespace IO
	{
		class FileManager
		{
		public:
			// Recursively searches for a file with the given name in the given directory
			static FilePath FindFile( const FilePath& a_File, const FilePath& a_Directory = GetWorkingDirectory() );
			// Recursively searches for a file with the given extension in the given directory.
			// a_Extension should include the '.' character. For example, ".png"
			static FilePath FindFileWithExtension( const FilePath& a_Extension, const FilePath& a_Directory = GetWorkingDirectory() );
			static const FilePath& GetWorkingDirectory();
		};
	}
}