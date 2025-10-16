#pragma once
#include <type_traits>

namespace Tridium {

	template<typename T>
	struct UnderlyingType
	{
		using Type = std::underlying_type_t<T>;
	};

	template<typename T>
	using UnderlyingTypeT = typename UnderlyingType<T>::Type;

	template<typename T>
	struct Hash
	{
		constexpr size_t operator()( const T& a_Value ) const noexcept
		{
			return std::hash<T>()( a_Value );
		}
	};

	template<typename T>
	struct Equal
	{
		constexpr bool operator()( const T& a_Lhs, const T& a_Rhs ) const noexcept
		{
			return a_Lhs == a_Rhs;
		}
	};

	template<typename T>
	struct NotEqual
	{
		constexpr bool operator()( const T& a_Lhs, const T& a_Rhs ) const noexcept
		{
			return !( Equal<T>()( a_Lhs, a_Rhs ) );
		}
	};

	template<typename T>
	using Allocator = std::allocator<T>;

} // namespace Tridium

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