#pragma once
#include <filesystem>
#include <Tridium/Containers/String.h>

namespace Tridium {
    namespace IO {

        namespace fs = std::filesystem;

        class FilePath : private fs::path
        {
        public:
            using fs::path::path;
            FilePath( const fs::path& a_Path ) : fs::path( a_Path ) {}

            operator const fs::path& ( ) const { return *this; }
            operator fs::path& ( ) { return *this; }

            FilePath operator+( const std::string& suffix ) const {
                return ToString() + suffix;
            }

            FilePath& operator+=( const std::string& suffix ) {
                fs::path::operator+=( suffix );
                return *this;
            }

            FilePath operator/( const FilePath& other ) const {
                return FilePath( static_cast<fs::path>( *this ) / static_cast<fs::path>( other ) );
            }

            bool operator==( const FilePath& other ) const {
                return static_cast<const std::filesystem::path&>( *this ) == static_cast<const fs::path&>( other );
            }

            bool operator!=( const FilePath& other ) const {
                return !( *this == other );
            }

            bool operator==( const std::string& other ) const {
                return ToString() == other;
            }

            bool operator!=( const std::string& other ) const {
                return ToString() == other;
            }

            bool operator==( const char* other ) const {
                return ToString() == other;
            }

            bool operator!=( const char* other ) const {
                return ToString() == other;
            }

            // Conversion to string with forward slashes
            //      E.g. "MyFolder/MyFile.txt"
			String ToString() const { return generic_string(); }

             // Conversion to WString with forward slashes
            //      E.g. "MyFolder/MyFile.txt"
			WString ToWString() const { return generic_wstring(); }

            void Append( const String& a_String ) { append( a_String ); }

            void ReplaceExtension( const String& a_Extension ) { replace_extension( a_Extension ); }

            FilePath GetParentPath() const { return parent_path(); }

            FilePath GetFilename() const { return filename(); }

            String GetFilenameWithoutExtension() const { return filename().replace_extension().string(); }

            FilePath GetExtension() const { return extension(); }

            bool HasParentPath() const { return has_parent_path(); }

            bool HasExtension() const { return has_extension(); }

            bool Exists() const { return fs::exists( *this ); }

            bool IsDirectory() const { return fs::is_directory( *this ); }

            bool IsFile() const { return fs::is_regular_file( *this ); }

            bool IsAbsolute() const { return is_absolute(); }

            bool IsRelative() const { return is_relative(); }

            bool IsEmpty() const { return empty(); }

            bool IsRoot() const { return root_path() == static_cast<const std::filesystem::path&>( *this ); }

            bool Remove() { return fs::remove( *this ); }

            static FilePath Absolute( const FilePath& a_Path ) { return fs::absolute( a_Path ); }

            static FilePath Relative( const FilePath& a_Path, const FilePath& a_Base ) { return fs::relative( a_Path, a_Base ); }

            static FilePath Canonical( const FilePath& a_Path ) { return fs::canonical( a_Path ); }

            static FilePath TempDirectoryPath() { return fs::temp_directory_path(); }

            static FilePath CurrentPath() { return fs::current_path(); }

            static FilePath FromPath( const fs::path& a_Path ) { return FilePath( a_Path ); }
        };

        class DirectoryEntry : public fs::directory_entry
        {

        };

        class DirectoryIterator : public fs::directory_iterator
        {
        public:
            DirectoryIterator( const FilePath& a_Path ) : fs::directory_iterator( a_Path.ToString() ) {}


        };

    }

	using IO::FilePath;

}

namespace std {
    template <>
    struct hash<Tridium::FilePath>
    {
        std::size_t operator()( const Tridium::FilePath& s ) const noexcept
        {
            return std::hash<filesystem::path>{}( s.ToString() );
        }
    };
}