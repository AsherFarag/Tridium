#pragma once
#include <type_traits>
#include <memory>

namespace Tridium {

	template<typename T>
	struct MemberPointerTraits
	{
		using ClassType = void;
		using MemberType = void;
	};

	// Member variable pointer
	template<typename Class, typename Member>
	struct MemberPointerTraits<Member Class::*>
	{
		using ClassType = Class;
		using MemberType = Member;
	};

	// Member function pointer
	template<typename Class, typename Ret, typename... Args>
	struct MemberPointerTraits<Ret( Class::* )( Args... )>
	{
		using ClassType = Class;
		using MemberType = Ret;
	};

	// Const member function pointer
	template<typename Class, typename Ret, typename... Args>
	struct MemberPointerTraits<Ret( Class::* )( Args... ) const>
	{
		using ClassType = Class;
		using MemberType = Ret;
	};

	// Static/free function taking object by reference
	template<typename Ret, typename Obj, typename... Args>
	struct MemberPointerTraits<Ret( * )( Obj&, Args... )>
	{
		using ClassType = std::remove_cvref_t<Obj>;
		using MemberType = Ret;
	};

	// Static/free function taking object by pointer
	template<typename Ret, typename Obj, typename... Args>
	struct MemberPointerTraits<Ret( * )( Obj*, Args... )>
	{
		using ClassType = std::remove_cvref_t<Obj>;
		using MemberType = Ret;
	};

	template<typename> 
	struct MemberFunctionTraits;

	template <typename _Return, typename _Object, typename... _Args>
	struct MemberFunctionTraits<_Return( _Object::* )( _Args... )>
	{
		using ReturnType = _Return;
		using ClassType = _Object;
		using ClassReference = _Object&;
		using ArgumentList = void;

		static constexpr size_t ArgCount = sizeof...( _Args );
	};

	template <typename _Return, typename _Object, typename... _Args>
	struct MemberFunctionTraits<_Return( _Object::* )( _Args... ) const>
	{
		using ReturnType = _Return;
		using ClassType = _Object;
		using ClassReference = const _Object&;
		using ArgumentList = void;

		static constexpr size_t ArgCount = sizeof...( _Args );
	};

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