#pragma once
#include <type_traits>
#include <Tridium/Utils/Macro.h>
#include <Tridium/Containers/String.h>
#include <source_location>

namespace Tridium {

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
	}


	template<typename T>
	consteval StringView GetTypeName() noexcept
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

}

namespace std { 

	template<typename T>
	struct get_member_function_type
	{
		using Type = void;
	};

	template<typename _Return, typename _Class, typename... _Args>
	struct get_member_function_type < _Return( _Class::* )( _Args... ) >
	{
		using Type = _Class;
	};

	template<typename _Return, typename _Class, typename... _Args>
	struct get_member_function_type < _Return( _Class::* const )( _Args... ) >
	{
		using Type = _Class;
	};

	template<typename _Return, typename _Class, typename... _Args>
	struct get_member_function_type < _Return( _Class::* volatile )( _Args... ) >
	{
		using Type = _Class;
	};

	template<typename _Return, typename _Class, typename... _Args>
	struct get_member_function_type < _Return( _Class::* const volatile )( _Args... ) >
	{
		using Type = _Class;
	};

	template<typename T>
	using get_member_function_type_t = typename get_member_function_type<T>::Type;

} // namespace std