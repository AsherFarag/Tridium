#pragma once

namespace Tridium {

	//=======================================================
	// IO namespace
	//  Contains static utility functions for file I/O that are platform-independent.
	namespace IO {

		// Check if a file exists
		extern bool FileExists( const String& filepath );

		// Read an entire file into a string
		extern String ReadFile( const String& filepath );

		// Read a binary file into a byte buffer
		extern Array<uint8_t> ReadBinaryFile( const String& filepath );

		// Write a string to a file (overwrites if exists)
		extern bool WriteFile( const String& filepath, const String& data );

		// Write a binary buffer to a file
		extern bool WriteBinaryFile( const String& filepath, const Array<uint8_t>& data );

		// Append a string to an existing file
		extern bool AppendToFile( const String& filepath, const String& data );

		// Get the size of a file in bytes
		extern size_t GetFileSize( const String& filepath );

		// Delete a file
		extern bool DeleteFile( const String& filepath );

		// Rename a file
		extern bool RenameFile( const String& oldPath, const String& newPath );

		// Create a directory (if it doesn’t exist)
		extern bool CreateDirectory( const String& dirPath );

		// Delete a directory and all contents
		extern bool DeleteDirectory( const String& dirPath );

		// List files in a directory
		extern Array<String> ListFilesInDirectory( const String& dirPath );

		// Get the file extension from a path
		extern String GetFileExtension( const String& filepath );

		// Get the filename (without directory path)
		extern String GetFileName( const String& filepath );

		// Get the directory path from a full filepath
		extern String GetDirectoryPath( const String& filepath );

	} // namespace IO

}// namespace Tridium