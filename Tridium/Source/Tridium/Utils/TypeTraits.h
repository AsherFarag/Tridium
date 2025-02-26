#pragma once
#include <type_traits>
#include <Tridium/Utils/Macro.h>
#include <Tridium/Containers/String.h>

namespace Tridium {

	template<typename T>
	constexpr StringView StrippedTypeName() noexcept
	{
	#if defined TRIDIUM_PRETTY_FUNCTION
		StringView prettyFunction{ TRIDIUM_PRETTY_FUNCTION };
		auto first = prettyFunction.find_first_not_of( ' ', prettyFunction.find_first_of( TRIDIUM_PRETTY_FUNCTION_PREFIX ) + 1 );
		auto value = prettyFunction.substr( first, prettyFunction.find_last_of( TRIDIUM_PRETTY_FUNCTION_SUFFIX ) - first );
		return value;
	#else
		return StringView{ "" };
	#endif
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