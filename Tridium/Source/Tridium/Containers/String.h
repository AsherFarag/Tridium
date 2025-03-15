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

	inline WString ToWString( const String& a_String )
	{
		return WString( a_String.begin(), a_String.end() );
	}

	inline String ToString( const WString& a_String )
	{
		return String( a_String.begin(), a_String.end() );
	}

} // namespace Tridium