#include "tripch.h"
#include "StringHelpers.h"

namespace Tridium::Util {
    std::string ToLowerCase( const std::string& str )
    {
        std::string result = str;
        std::transform( result.begin(), result.end(), result.begin(),
            +[]( unsigned char c ) { return std::tolower( c ); } );
        return result;
    }
    std::string ToUpperCase( const std::string& str )
    {
        std::string result = str;
        std::transform( result.begin(), result.end(), result.begin(),
            +[]( unsigned char c ) { return std::toupper( c ); } );
        return result;
    }
}