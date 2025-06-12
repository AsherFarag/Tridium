#pragma once

namespace Tridium {

	//=======================================================
	// IO namespace
	//  Contains static utility functions for file I/O that are platform-independent.
	namespace IO {

		// Check if a file exists
		extern bool FileExists( StringView a_Filepath );

		// Read an entire file into a string
		extern String ReadFile( StringView a_Filepath );

		// Read a binary file into a byte buffer
		extern Array<uint8_t> ReadBinaryFile( StringView a_Filepath );

		// Write a string to a file (overwrites if exists)
		extern bool WriteFile( StringView a_Filepath, StringView data );

		// Write a binary buffer to a file
		extern bool WriteBinaryFile( StringView a_Filepath, Span<const uint8_t> a_Data );

		// Append a string to an existing file
		extern bool AppendToFile( StringView a_Filepath, StringView a_Data );

		// Get the size of a file in bytes
		extern size_t GetFileSize( StringView a_Filepath );

		// Delete a file
		extern bool DeleteFile( StringView a_Filepath );

		// Rename a file
		extern bool RenameFile( StringView a_FilePath, StringView newPath );

		// Create a directory (if it doesn’t exist)
		extern bool CreateDirectory( StringView a_DirectoryPath );

		// Delete a directory and all contents
		extern bool DeleteDirectory( StringView a_DirectoryPath );

		// List files in a directory
		extern Array<String> ListFilesInDirectory( StringView a_DirectoryPath, StringView a_Filter = "" );

		// Get the file extension from a path
		extern String GetFileExtension( StringView a_Filepath );

		// Get the filename (without directory path)
		extern String GetFileName( StringView a_Filepath );

		// Get the directory path from a full a_Filepath
		extern String GetDirectoryPath( StringView a_Filepath );

	} // namespace IO

}// namespace Tridium