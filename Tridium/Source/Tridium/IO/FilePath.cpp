#include "tripch.h"
#include "FilePath.h"
#include <random>


namespace Tridium::IO {

    FilePath FilePath::UniquePath( StringView a_Pattern )
    {
        static thread_local std::mt19937_64 rng{ std::random_device{}( ) };
        static std::uniform_int_distribution<int> dist( 0, 15 );

        auto HexDigit = [ & ]() -> char
        {
            int v = dist( rng );
            return ( v < 10 ) ? Cast< char >( '0' + v )
                              : Cast< char >( 'a' + ( v - 10 ) );
        };

		static thread_local char buffer[ 1024 ];
		size_t length = 0;
        for ( char c : a_Pattern )
        {
            if ( c == '%' )
            {
                buffer[ length++ ] = HexDigit();
            }
            else
            {
                buffer[ length++ ] = c;
            }
		}

		return FilePath( StringView( buffer, length ) );
    }

}