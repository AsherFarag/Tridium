#pragma once
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>

namespace Tridium {

    namespace Util {
        std::string ToLowerCase( const std::string& str ) {
            std::string result = str;
            std::transform( result.begin(), result.end(), result.begin(),
                []( unsigned char c ) { return std::tolower( c ); } );
            return result;
        }

        std::string ToUpperCase( const std::string& str ) {
            std::string result = str;
            std::transform( result.begin(), result.end(), result.begin(),
                []( unsigned char c ) { return std::toupper( c ); } );
            return result;
        }
    }

}

