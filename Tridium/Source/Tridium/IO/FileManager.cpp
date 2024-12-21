#include "tripch.h"
#include "FileManager.h"

namespace Tridium::IO {

    FilePath FileManager::FindFile( const FilePath& a_File, const FilePath& a_Directory )
    {
		// If the file is an absolute path, return it
		if ( a_File.IsAbsolute() )
		{
			TE_CORE_WARN( "FileManager::FindFile: File is already an absolute path {0}", a_File.ToString() );
			return a_File;
		}

		for ( const auto& entry : fs::directory_iterator( (const fs::path&)a_Directory ) )
		{
			if ( entry.is_regular_file() && static_cast<FilePath>( entry.path().filename() ) == a_File.GetFilename() )
			{
				return FilePath::FromPath( entry.path() );
			}
		}

		return {};
    }

	FilePath FileManager::FindFileWithExtension( const FilePath& a_Extension, const FilePath& a_Directory )
	{
		for ( const auto& entry : fs::recursive_directory_iterator( (const fs::path&)a_Directory ) )
		{
			const fs::path& path = entry.path();
			if ( entry.is_directory() || !path.has_extension() )
			{
				continue;
			}

			IO::FilePath extension = entry.path().extension();
			if ( extension == a_Extension )
			{
				return path;
			}
		}

		return {};
	}

	const FilePath& FileManager::GetWorkingDirectory()
	{
		static FilePath workingDirectory = fs::current_path();
		return workingDirectory;
	}

}
