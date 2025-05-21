#pragma once
#include <string>

namespace Tridium {

	// Basic string types

	template<typename T>
	using TString = std::basic_string<T, std::char_traits<T>, std::allocator<T>>;
	template<typename T>
	using TStringView = std::basic_string_view<T, std::char_traits<T>>;
	template<typename T>
	using TStringStream = std::basic_stringstream<T>;

	// String

	using String = std::string;
	using StringView = std::string_view;
	using StringStream = std::stringstream;

	// Wide string

	using WString = std::wstring;
	using WStringView = std::wstring_view;
	using WStringStream = std::wstringstream;

	namespace Util {
		// Converts a string to lowercase.
		template<typename T>
		inline void ToLower( TString<T>& a_String )
		{
			for ( auto& c : a_String )
				c = std::tolower( c );
		}

		// Converts a string to uppercase.
		template<typename T>
		inline void ToUpper( TString<T>& a_String )
		{
			for ( auto& c : a_String )
				c = std::toupper( c );
		}
	}
	

	template<typename T>
	inline String ToString( const T& a_Value )
	{
		return std::to_string( a_Value );
	}

	template<typename T>
	inline WString ToWString( T&& a_Value )
	{
		const auto str = ToString( std::forward<T>( a_Value ) );
		return WString( str.begin(), str.end() );
	}

	template<>
	inline String ToString( const WString& a_String )
	{
		return String( a_String.begin(), a_String.end() );
	}

	template<>
	inline String ToString( const WStringView& a_String )
	{
		return String( a_String.begin(), a_String.end() );
	}

	template<>
	inline WString ToWString( const String& a_String )
	{
		return WString( a_String.begin(), a_String.end() );
	}

	template<>
	inline WString ToWString( const StringView& a_String )
	{
		return WString( a_String.begin(), a_String.end() );
	}

	template<typename T>
	inline bool FromString( StringView a_String, T* o_Value );

} // namespace Tridium