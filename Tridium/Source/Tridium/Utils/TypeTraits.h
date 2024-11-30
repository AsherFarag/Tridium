#pragma once
#include <type_traits>

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