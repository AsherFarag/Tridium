#pragma once
#include <Tridium/Containers/String.h>
#include <Tridium/Containers/Array.h>

namespace Tridium::Util {

	// Converts a string to lowercase.
	template<typename T>
	inline void ToLower( BasicString<T>& o_String )
	{
		for ( auto& c : o_String )
			c = std::tolower( c );
	}

	// Converts a string to uppercase.
	template<typename T>
	inline void ToUpper( BasicString<T>& o_String )
	{
		for ( auto& c : o_String )
			c = std::toupper( c );
	}

	// Converts a string to lowercase.
	template<typename T>
	inline BasicString<T> ToLower( BasicStringView<T> a_String )
	{
		BasicString<T> result( a_String );
		for ( auto& c : result )
			c = std::tolower( c );
		return result;
	}

	// Converts a string to uppercase.
	template<typename T>
	inline BasicString<T> ToUpper( BasicStringView<T> a_String )
	{
		BasicString<T> result( a_String );
		for ( auto& c : result )
			c = std::toupper( c );
		return result;
	}

	// Splits a string into an array of sub string views based on a delimiter.
	template<typename _Elem, typename _Traits = std::char_traits<_Elem>>
	inline Array<BasicStringView<_Elem, _Traits>> Split( BasicStringView<_Elem, _Traits> a_String, BasicStringView<_Elem, _Traits> a_Delimiter )
	{
		Array<BasicStringView<_Elem, _Traits>> result;
		size_t start = 0;
		size_t end = a_String.find( a_Delimiter );
		while ( end != BasicStringView<_Elem, _Traits>::npos )
		{
			result.PushBack( a_String.substr( start, end - start ) );
			start = end + a_Delimiter.length();
			end = a_String.find( a_Delimiter, start );
		}
		result.PushBack( a_String.substr( start, end ) );
		return result;
	}

}