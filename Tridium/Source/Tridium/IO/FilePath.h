#pragma once
#include <filesystem>
#include <string>

namespace Tridium::IO {

    namespace fs = std::filesystem;

    class FilePath : private fs::path
    {
    public:
        using fs::path::path;
        FilePath( const fs::path& a_Path ) : fs::path( a_Path ) {}

        operator const fs::path&() const { return *this; }
        operator std::string() const { return ToString(); }

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
        std::string ToString() const { return generic_string(); }

        void Append( const std::string& a_String ) { append( a_String ); }

        FilePath GetParentPath() const { return parent_path(); }

        FilePath GetFilename() const { return filename(); }

        FilePath GetExtension() const { return extension(); }

        bool HasParentPath() const { return has_parent_path(); }

        bool HasExtension() const { return has_extension(); }

        bool Exists() const { return fs::exists( *this ); }

        bool IsDirectory() const { return fs::is_directory( *this ); }

        bool IsFile() const { return fs::is_regular_file( *this ); }

        static FilePath FromPath( const IO::FilePath& path ) { return FilePath( path ); }
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

namespace std {
    template <>
    struct hash<Tridium::IO::FilePath>
    {
        std::size_t operator()( const Tridium::IO::FilePath& s ) const noexcept
        {
            return std::hash<std::string>{}( s.ToString() );
        }
    };
}