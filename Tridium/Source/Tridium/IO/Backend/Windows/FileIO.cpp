#include "tripch.h"
#if CONFIG_PLATFORM_WINDOWS
#include <Tridium/IO/FileIO.h>
#include <fstream>
#include <filesystem>

namespace Tridium {

	bool IO::FileExists( StringView a_Filepath )
	{
		return std::filesystem::exists( a_Filepath );
	}

	String IO::ReadFile( StringView a_Filepath )
	{
		std::ifstream file( a_Filepath.data() );
		if ( !file.is_open() )
		{
			return {};
		}

		StringStream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	Array<uint8_t> IO::ReadBinaryFile( StringView a_Filepath )
	{
		std::ifstream file( a_Filepath.data(), std::ios::binary );
		if ( !file.is_open() )
		{
			return {};
		}

		file.seekg( 0, std::ios::end );
		size_t size = file.tellg();
		file.seekg( 0, std::ios::beg );

		Array<uint8_t> buffer( size );
		file.read( ReinterpretCast<char*>( buffer.Data() ), size );
		return buffer;
	}

	bool IO::WriteFile( StringView a_Filepath, StringView a_Data )
	{
		std::ofstream file( a_Filepath.data() );
		if ( !file.is_open() )
		{
			return false;
		}

		file << a_Data;
		return true;
	}

	bool IO::WriteBinaryFile( StringView a_Filepath, Span<const uint8_t> a_Data )
	{
		std::ofstream file( a_Filepath.data(), std::ios::binary);
		if ( !file.is_open() )
		{
			return false;
		}
		file.write( ReinterpretCast<const char*>( a_Data.data() ), a_Data.size() );
		return true;
	}

	bool IO::AppendToFile( StringView a_Filepath, StringView a_Data )
	{
		std::ofstream file( a_Filepath.data(), std::ios::app );
		if ( !file.is_open() )
		{
			return false;
		}
		file << a_Data;
		return true;
	}

	size_t IO::GetFileSize( StringView a_Filepath )
	{
		std::ifstream file( a_Filepath.data(), std::ios::binary | std::ios::ate );
		if ( !file.is_open() )
		{
			return 0;
		}

		return file.tellg();
	}

	bool IO::DeleteFile( StringView a_Filepath )
	{
		return std::filesystem::remove( a_Filepath );
	}

	bool IO::RenameFile( StringView oldPath, StringView newPath )
	{
		std::error_code ec;
		std::filesystem::rename( oldPath, newPath, ec );

		return !ec;
	}

	bool IO::CreateDirectory( StringView dirPath )
	{
		return std::filesystem::create_directories( dirPath );
	}

	bool IO::DeleteDirectory( StringView dirPath )
	{
		return std::filesystem::remove_all( dirPath );
	}

	Array<String> IO::ListFilesInDirectory( StringView a_DirectoryPath, StringView a_Filter )
	{
		std::filesystem::path dirPath( a_DirectoryPath );
		if ( !std::filesystem::exists( dirPath ) || !std::filesystem::is_directory( dirPath ) )
		{
			return {};
		}

		Array<String> files;
		for ( const auto& entry : std::filesystem::directory_iterator( dirPath ) )
		{
			if ( entry.is_regular_file() && (a_Filter.empty() || entry.path().extension() == a_Filter) )
			{
				files.PushBack( entry.path().string() );
			}
		}
		return files;
	}

	String IO::GetFileExtension( StringView a_Filepath )
	{
		return std::filesystem::path( a_Filepath ).extension().string();
	}

	String IO::GetFileName( StringView a_Filepath )
	{
		return std::filesystem::path( a_Filepath ).filename().string();
	}

	String IO::GetDirectoryPath( StringView a_Filepath )
	{
		return std::filesystem::path( a_Filepath ).parent_path().string();
	}

} // namespace Tridium

#endif