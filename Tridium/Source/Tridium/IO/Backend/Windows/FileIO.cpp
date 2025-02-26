#include "tripch.h"
#if CONFIG_PLATFORM_WINDOWS
#include <Tridium/IO/FileIO.h>
#include <fstream>

namespace Tridium {

	bool IO::FileExists( const String& filepath )
	{
		return std::filesystem::exists( filepath );
	}

	String IO::ReadFile( const String& filepath )
	{
		std::ifstream file( filepath );
		if ( !file.is_open() )
		{
			return {};
		}

		StringStream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	Array<uint8_t> IO::ReadBinaryFile( const String& filepath )
	{
		std::ifstream file( filepath, std::ios::binary );
		if ( !file.is_open() )
		{
			return {};
		}

		file.seekg( 0, std::ios::end );
		size_t size = file.tellg();
		file.seekg( 0, std::ios::beg );

		Array<uint8_t> buffer( size );
		file.read( reinterpret_cast<char*>( buffer.Data() ), size );
		return buffer;
	}

	bool IO::WriteFile( const String& filepath, const String& data )
	{
		std::ofstream file( filepath );
		if ( !file.is_open() )
		{
			return false;
		}

		file << data;
		return true;
	}

	bool IO::WriteBinaryFile( const String& filepath, const Array<uint8_t>& data )
	{
		std::ofstream file( filepath, std::ios::binary );
		if ( !file.is_open() )
		{
			return false;
		}
		file.write( reinterpret_cast<const char*>( data.Data() ), data.Size() );
		return true;
	}

	bool IO::AppendToFile( const String& filepath, const String& data )
	{
		std::ofstream file( filepath, std::ios::app );
		if ( !file.is_open() )
		{
			return false;
		}
		file << data;
		return true;
	}

	size_t IO::GetFileSize( const String& filepath )
	{
		std::ifstream file( filepath, std::ios::binary | std::ios::ate );
		if ( !file.is_open() )
		{
			return 0;
		}

		return file.tellg();
	}

	bool IO::DeleteFile( const String& filepath )
	{
		return std::filesystem::remove( filepath );
	}

	bool IO::RenameFile( const String& oldPath, const String& newPath )
	{
		std::error_code ec;
		std::filesystem::rename( oldPath, newPath, ec );

		return !ec;
	}

	bool IO::CreateDirectory( const String& dirPath )
	{
		return std::filesystem::create_directories( dirPath );
	}

	bool IO::DeleteDirectory( const String& dirPath )
	{
		return std::filesystem::remove_all( dirPath );
	}

	Array<String> IO::ListFilesInDirectory( const String& dirPath )
	{
		Array<String> files;
		for ( const auto& entry : std::filesystem::directory_iterator( dirPath ) )
		{
			files.PushBack( entry.path().string() );
		}
		return files;
	}

	String IO::GetFileExtension( const String& filepath )
	{
		return std::filesystem::path( filepath ).extension().string();
	}

	String IO::GetFileName( const String& filepath )
	{
		return std::filesystem::path( filepath ).filename().string();
	}

	String IO::GetDirectoryPath( const String& filepath )
	{
		return std::filesystem::path( filepath ).parent_path().string();
	}

} // namespace Tridium

#endif