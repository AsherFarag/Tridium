#pragma once
#include <string>
#include <string_view>
#include <Tridium/Utils/TypeTraits.h>
#include <source_location>

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

	template<typename T>
	struct Equal<BasicStringView<T>>
	{
		using is_transparent = void;

		bool operator()( const BasicStringView<T>& a_Lhs, const BasicStringView<T>& a_Rhs ) const noexcept
		{
			return a_Lhs == a_Rhs;
		}
	};

	namespace Detail {

		template<typename T>
		consteval StringView GetFunctionName()
		{
		#if defined(__clang__) && defined(_WIN32)
			// clang-cl returns function_name() as __FUNCTION__ instead of __PRETTY_FUNCTION__
			return StringView{ __PRETTY_FUNCTION__ };
		#else
			return StringView{ std::source_location::current().function_name() };
		#endif
		}

		struct TypeNameDetector
		{
		};
	} // namespace Detail


	template<typename T>
	constexpr StringView GetTypeName() noexcept
	{
	#if defined(__GNUC__) || defined(__clang__)
		constexpr auto detectorName = get_function_name<TypeNameDetector>();
		constexpr auto dummy = StringView( "T = " );
		constexpr auto dummyBegin = detectorName.find( dummy ) + dummy.size();
		constexpr auto dummy2 = StringView( "TypeNameDetector" );
		constexpr auto dummySuffixLength = detectorName.size() - detectorName.find( dummy2 ) - dummy2.size();

		constexpr auto typeNameRaw = get_function_name<T>();
		return typeNameRaw.substr( dummyBegin, typeNameRaw.size() - dummyBegin - dummySuffixLength );
	#else
		constexpr auto detectorName = Detail::GetFunctionName<Detail::TypeNameDetector>();
		constexpr auto dummy = StringView( "struct Tridium::Detail::TypeNameDetector" );
		constexpr auto dummyBegin = detectorName.find( dummy );
		constexpr auto dummySuffixLength = detectorName.size() - dummyBegin - dummy.size();

		auto typeNameRaw = Detail::GetFunctionName<T>();
		auto typeName =
			typeNameRaw.substr( dummyBegin, typeNameRaw.size() - dummyBegin - dummySuffixLength );
		if ( auto s = StringView( "struct " ); typeName.starts_with( s ) )
		{
			typeName.remove_prefix( s.size() );
		}
		if ( auto s = StringView( "class " ); typeName.starts_with( s ) )
		{
			typeName.remove_prefix( s.size() );
		}
		return typeName;
	#endif
	}

	// Get the type name without the namespace
	template<typename T>
	consteval StringView GetStrippedTypeName()
	{
		auto typeName = GetTypeName<T>();
		if ( auto s = typeName.find_last_of( ':' ); s != StringView::npos )
		{
			typeName.remove_prefix( s + 1 );
		}
		return typeName;
	}

} // namespace Tridium