#pragma once
#include <string>

namespace Tridium {

	// Basic string types

	template<typename _Elem, typename _Traits = std::char_traits<_Elem>, typename _Alloc = std::allocator<_Elem>>
	using BasicString = std::basic_string<_Elem, _Traits, _Alloc>;
	template<typename _Elem, typename _Traits = std::char_traits<_Elem>>
	using BasicStringView = std::basic_string_view<_Elem, _Traits>;
	template<typename _Elem, typename _Traits = std::char_traits<_Elem>, typename _Alloc = std::allocator<_Elem>>
	using BasicStringStream = std::basic_stringstream<_Elem, _Traits, _Alloc>;

	using String = BasicString<char>;
	using StringView = BasicStringView<char>;
	using StringStream = BasicStringStream<char>;

	using WString = BasicString<wchar_t>;
	using WStringView = BasicStringView<wchar_t>;
	using WStringStream = BasicStringStream<wchar_t>;

	template<typename _Elem, size_t _Count>
	struct BasicStringLiteral
	{
		_Elem Data[ _Count ];
		static constexpr size_t Size = _Count - 1; // Exclude null terminator

		constexpr BasicStringLiteral( const _Elem( &a_String )[ _Count ] )
		{
			std::copy_n( a_String, _Count, Data );
		}

		constexpr operator BasicStringView<_Elem>() const noexcept
		{
			return BasicStringView<_Elem>( Data );
		}
	};

	template<size_t _Count>
	using StringLiteral = BasicStringLiteral<char, _Count>;
	template<size_t _Count>
	using WStringLiteral = BasicStringLiteral<wchar_t, _Count>;

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


	namespace Detail {

		template<typename... _Bases>
		struct Overload : _Bases...
		{
			using is_transparent = void;
			using _Bases::operator()...;
		};

		struct CharPtrHash
		{
			auto operator()( const char* a_String ) const noexcept -> std::size_t
			{
				return std::hash<StringView>{}(a_String);
			}
		};
	}

	// Transparent hash function that can accept both String and StringView types
	using TransparentStringHash = Detail::Overload<
		std::hash<String>,
		std::hash<StringView>,
		Detail::CharPtrHash
	>;

} // namespace Tridium