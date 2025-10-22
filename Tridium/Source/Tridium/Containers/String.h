#pragma once
#include <string>
#include <Tridium/Containers/StringView.h>

namespace Tridium {

	// Basic string types

	template<typename _Elem, typename _Traits = std::char_traits<_Elem>, typename _Alloc = std::allocator<_Elem>>
	using BasicString = std::basic_string<_Elem, _Traits, _Alloc>;
	template<typename _Elem, typename _Traits = std::char_traits<_Elem>, typename _Alloc = std::allocator<_Elem>>
	using BasicStringStream = std::basic_stringstream<_Elem, _Traits, _Alloc>;

	using String = BasicString<char>;
	using StringStream = BasicStringStream<char>;

	using WString = BasicString<wchar_t>;
	using WStringStream = BasicStringStream<wchar_t>;

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
			// Enable transparent hashing/equality comparison
			using is_transparent = void; 
			using _Bases::operator()...;
		};

		template<typename T>
		struct RawStringHash
		{
			size_t operator()( const T* a_String ) const noexcept
			{
				return std::hash<StringView>{}( a_String );
			}
		};

	} // namespace Detail

	template<typename T>
	struct Hash<BasicString<T>> : Detail::Overload<
		std::hash<BasicString<T>>,
		std::hash<BasicStringView<T>>,
		Detail::RawStringHash<T>> 
	{
		using is_transparent = void;
	};

	template<typename T>
	struct Hash<BasicStringView<T>> : Detail::Overload<
		std::hash<BasicString<T>>,
		std::hash<BasicStringView<T>>,
		Detail::RawStringHash<T>>
	{
		using is_transparent = void;
	};

	template<typename T>
	struct Equal<BasicString<T>>
	{
		using is_transparent = void;

		bool operator()( const BasicStringView<T>& a_Lhs, const BasicStringView<T>& a_Rhs ) const noexcept
		{
			return a_Lhs == a_Rhs;
		}
	};

} // namespace Tridium