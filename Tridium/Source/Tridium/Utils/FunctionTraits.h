#pragma once
#include <utility>
#include <tuple>
#include <type_traits>
#include <functional>

namespace apl {

	enum function_flags
	{
		_none		= 0b00000000,
		_static		= 0b00000001,
		_member		= 0b00000010,
		_lambda		= 0b00000100,
		_const		= 0b00001000,
		_volatile	= 0b00010000,
		_noexcept	= 0b00100000,
		_lvalue		= 0b01000000,
		_rvalue		= 0b10000000,
	};

	constexpr function_flags operator|( const function_flags l, const function_flags r ) { return static_cast< function_flags >( static_cast< int >( l ) | static_cast< int >( r ) ); }
	constexpr function_flags operator&( const function_flags l, const function_flags r ) { return static_cast< function_flags >( static_cast< int >( l ) & static_cast< int >( r ) ); }

	template < typename >
	struct tuple_as_args;

	template < typename... >
	struct args_list {};

	template < typename >
	struct args_as_tuple { using type = void; };

	template < typename... _Args >
	struct args_as_tuple< args_list< _Args... > > { using type = std::tuple< _Args... >; };

	template < typename _Args >
	using args_as_tuple_t = typename args_as_tuple< _Args >::type;

	template < typename >
	struct decayed_args { using type = void; };

	template < typename... _Args >
	struct decayed_args< args_list< _Args... > > { using type = args_list< std::decay_t< _Args >... >; };

	template < typename _Args >
	using decayed_args_t = typename decayed_args< _Args >::type;

	template < typename >
	struct tuple_as_args { using type = void; };

	template < typename... _Args >
	struct tuple_as_args< std::tuple< _Args... > > { using type = args_list< _Args... >; };

	template < typename _Tuple >
	using tuple_as_args_t = typename tuple_as_args< _Tuple >::type;

	template < typename T, typename = void >
	struct is_lambda : std::false_type {};

	template < typename T >
	struct is_lambda< T, std::void_t< decltype( &T::operator() ) > > : std::true_type {};

	template < typename T >
	static constexpr bool is_lambda_v = is_lambda< T >::value;

	template < typename T >
	struct function_traits;

	template < typename T >
	struct lambda_traits
	{
		using return_type = typename function_traits< decltype( &T::operator() ) >::return_type;
		using args_type = typename function_traits< decltype( &T::operator() ) >::args_type;
		using signature_type = typename function_traits< decltype( &T::operator() ) >::signature_type;
		using object_type = T;

		static constexpr function_flags flags = ( function_flags )( _lambda | function_traits< decltype( &T::operator() ) >::flags & ~_member );
	};

	template < typename T >
	struct function_traits : std::conditional_t< is_lambda_v< T >, lambda_traits< T >, function_traits< void > > {};

	template <>
	struct function_traits< void >
	{
		using return_type = void;
		using args_type = void;
		using signature_type = void;
		using object_type = void;

		static constexpr function_flags flags = _none;
	};

	template < typename _Return, typename... _Args >
	struct function_traits< _Return( _Args... ) >
	{
		using return_type = _Return;
		using args_type = args_list< _Args... >;
		using signature_type = _Return( _Args... );
		using object_type = void;

		static constexpr function_flags flags = _none;
	};

#define MEMBER_FUNCTION_TRAITS( Flags, Traits ) \
	template < typename _Object, typename _Return, typename... _Args > \
	struct function_traits< _Return( _Object::* )( _Args... ) Traits > \
	{ \
		using return_type = _Return; \
		using args_type = args_list< _Args... >; \
		using signature_type = _Return( _Args... ); \
		using object_type = _Object; \
		static constexpr function_flags flags = _member | Flags; \
	};

	MEMBER_FUNCTION_TRAITS( _none, );
	MEMBER_FUNCTION_TRAITS( _lvalue, & );
	MEMBER_FUNCTION_TRAITS( _rvalue, && );
	MEMBER_FUNCTION_TRAITS( _const, const );
	MEMBER_FUNCTION_TRAITS( _volatile, volatile );
	MEMBER_FUNCTION_TRAITS( _const | _volatile, const volatile );
	MEMBER_FUNCTION_TRAITS( _const | _lvalue, const& );
	MEMBER_FUNCTION_TRAITS( _volatile | _lvalue, volatile& );
	MEMBER_FUNCTION_TRAITS( _const | _volatile | _lvalue, const volatile& );
	MEMBER_FUNCTION_TRAITS( _const | _rvalue, const&& );
	MEMBER_FUNCTION_TRAITS( _volatile | _rvalue, volatile&& );
	MEMBER_FUNCTION_TRAITS( _const | _volatile | _rvalue, const volatile&& );
	MEMBER_FUNCTION_TRAITS( _const | _noexcept, const noexcept );
	MEMBER_FUNCTION_TRAITS( _volatile | _noexcept, volatile noexcept );
	MEMBER_FUNCTION_TRAITS( _const | _volatile | _noexcept, const volatile noexcept );
	MEMBER_FUNCTION_TRAITS( _const | _lvalue | _noexcept, const& noexcept );
	MEMBER_FUNCTION_TRAITS( _volatile | _lvalue | _noexcept, volatile& noexcept );
	MEMBER_FUNCTION_TRAITS( _const | _volatile | _lvalue | _noexcept, const volatile& noexcept );
	MEMBER_FUNCTION_TRAITS( _const | _rvalue | _noexcept, const&& noexcept );
	MEMBER_FUNCTION_TRAITS( _volatile | _rvalue | _noexcept, volatile&& noexcept );
	MEMBER_FUNCTION_TRAITS( _const | _volatile | _rvalue | _noexcept, const volatile&& noexcept );

#undef MEMBER_FUNCTION_TRAITS

	template < typename _Return, typename... _Args >
	struct function_traits< _Return( * )( _Args... ) >
	{
		using return_type = _Return;
		using args_type = args_list< _Args... >;
		using signature_type = _Return( _Args... );
		using object_type = void;
		static constexpr function_flags flags = _static;
	};

	template < typename T >
	static constexpr bool is_static_fn_v = function_traits< T >::flags & _static;

	template < typename T >
	struct is_static_function : std::bool_constant< is_static_fn_v< T > > {};

	template < typename T >
	static constexpr bool is_member_fn_v = function_traits< T >::flags & _member;

	template < typename T >
	struct is_member_fn : std::bool_constant< is_member_fn_v< T > > {};

	template < typename T >
	static constexpr bool is_capture_lambda_v = is_lambda_v< T > && !std::is_convertible_v< T, std::add_pointer_t< typename function_traits< T >::signature_type > >;

	template < typename T >
	struct is_capture_lambda : std::bool_constant< is_capture_lambda_v< T > > {};

	template < typename _Object, typename _Fn, typename = typename function_traits< _Fn >::args_type, typename = void >
	struct is_member_fn_compatible : std::false_type {};

	template < typename _Object, typename _Fn, typename... _Args >
	struct is_member_fn_compatible< _Object, _Fn, args_list< _Args... >, std::void_t< decltype( std::mem_fn( std::declval< _Fn >() )( std::declval< std::conditional_t< std::is_rvalue_reference_v< _Object >, _Object, _Object& > >(), std::declval< _Args >()... ) ) > > : std::true_type {};

	template < typename _Object, typename _Fn >
	static constexpr bool is_member_fn_compatible_v = is_member_fn_compatible< _Object, _Fn >::value;

	template < size_t _Index, typename T >
	struct arg { using type = typename arg< _Index, typename function_traits< T >::signature_type >::type; };

	template < size_t _Index, typename... _Args >
	struct arg< _Index, args_list< _Args... > > { using type = std::tuple_element_t< _Index, std::tuple< _Args... > >; };

	template < size_t _Index, typename _Return, typename... _Args >
	struct arg< _Index, _Return( _Args... ) > : arg< _Index, args_list< _Args... > > {};

	template < size_t _Index, typename T >
	using arg_t = typename arg< _Index, T >::type;

	template < typename T >
	struct get_fn_arity : get_fn_arity< typename function_traits< T >::signature_type > {};

	template < typename... _Args >
	struct get_fn_arity< args_list< _Args... > > : std::integral_constant< size_t, sizeof...( _Args ) > {};

	template < typename _Return, typename... _Args >
	struct get_fn_arity< _Return( _Args... ) > : get_fn_arity< args_list< _Args... > > {};

	template < typename T >
	static constexpr size_t get_fn_arity_v = get_fn_arity< T >::value;

	template < typename T >
	using get_fn_return_t = typename function_traits< T >::return_type;

	template < typename T >
	struct get_fn_return { using type = get_fn_return_t< T >; };

	template < typename T >
	using get_fn_args_t = typename function_traits< T >::args_type;

	template < typename T >
	struct get_fn_args { using type = get_fn_args_t< T >; };

	template < typename T >
	using get_fn_sig_t = typename function_traits< T >::signature_type;

	template < typename T >
	struct get_fn_sig { using type = get_fn_sig_t< T >; };

	template < typename T >
	using get_fn_object_t = typename function_traits< T >::object_type;

	template < typename T >
	struct get_fn_object { using type = get_fn_object_t< T >; };

	template < typename T >
	static constexpr bool is_member_fn_const_v = function_traits< T >::flags & _const;

	template < typename T >
	struct is_member_fn_const : std::bool_constant< is_member_fn_const_v< T > > {};

	template < typename T >
	static constexpr bool is_member_fn_volatile_v = function_traits< T >::flags & _volatile;

	template < typename T >
	struct is_member_fn_volatile : std::bool_constant< is_member_fn_volatile_v< T > > {};

	template < typename T >
	static constexpr bool is_member_fn_rvalue_v = function_traits< T >::flags & _rvalue;

	template < typename T >
	struct is_member_fn_rvalue : std::bool_constant< is_member_fn_rvalue_v< T > > {};

	template < typename T >
	static constexpr bool is_member_fn_lvalue_v = function_traits< T >::flags & _lvalue; \

	template < typename T >
	struct is_member_fn_lvalue : std::bool_constant< is_member_fn_lvalue_v< T > > {};

	template < typename T >
	static constexpr bool is_member_fn_noexcept_v = function_traits< T >::flags & _noexcept;

	template < typename T >
	struct is_member_fn_noexcept : std::bool_constant< is_member_fn_noexcept_v< T > > {};

	template < typename _Generic, typename _Signature, typename = void >
	struct has_mutable_lambda_op : std::false_type {}; // operator()

	template < typename _Generic, typename _Signature, typename = void >
	struct has_lambda_op : std::false_type {}; // operator() const

	template < typename _Generic, typename _Signature, typename = void >
	struct has_mutable_noexcept_lambda_op : std::false_type {}; // operator() noexcept

	template < typename _Generic, typename _Signature, typename = void >
	struct has_noexcept_lambda_op : std::false_type {}; // operator() const noexcept

	template < typename _Generic, typename _Return, typename... _Args >
	struct has_mutable_lambda_op< _Generic, _Return( _Args... ), std::void_t< decltype( static_cast< _Return( _Generic::* )( _Args... ) >( &_Generic::operator() ) ) > > : std::true_type {};

	template < typename _Generic, typename _Return, typename... _Args >
	struct has_lambda_op< _Generic, _Return( _Args... ), std::void_t< decltype( static_cast< _Return( _Generic::* )( _Args... ) const >( &_Generic::operator() ) ) > > : std::true_type {};

	template < typename _Generic, typename _Return, typename... _Args >
	struct has_mutable_noexcept_lambda_op< _Generic, _Return( _Args... ), std::void_t< decltype( static_cast< _Return( _Generic::* )( _Args... ) noexcept >( &_Generic::operator() ) ) > > : std::true_type {};

	template < typename _Generic, typename _Return, typename... _Args >
	struct has_noexcept_lambda_op< _Generic, _Return( _Args... ), std::void_t< decltype( static_cast< _Return( _Generic::* )( _Args... ) const noexcept >( &_Generic::operator() ) ) > > : std::true_type {};

	template < typename _Generic, typename _Signature >
	static constexpr bool has_mutable_lambda_op_v = has_mutable_lambda_op< _Generic, _Signature >::value;

	template < typename _Generic, typename _Signature >
	static constexpr bool has_lambda_op_v = has_lambda_op< _Generic, _Signature >::value;

	template < typename _Generic, typename _Signature >
	static constexpr bool has_mutable_noexcept_lambda_op_v = has_mutable_noexcept_lambda_op< _Generic, _Signature >::value;

	template < typename _Generic, typename _Signature >
	static constexpr bool has_noexcept_lambda_op_v = has_noexcept_lambda_op< _Generic, _Signature >::value;

	template < typename _Generic, typename = std::conditional_t< is_lambda_v< _Generic >, get_fn_sig_t< _Generic >, void > >
	struct get_lambda_op
	{
		static constexpr void* value = nullptr;
		using type = void;
	};

	template < typename _Generic, typename _Return, typename... _Args >
	struct get_lambda_op< _Generic, _Return( _Args... ) >
	{
		static constexpr auto value = []
			{
				if constexpr ( has_mutable_lambda_op_v< _Generic, _Return( _Args... ) > )
				{
					return static_cast< _Return( _Generic::* )( _Args... ) >( &_Generic::operator() );
				}

				else if constexpr ( has_lambda_op_v< _Generic, _Return( _Args... ) > )
				{
					return static_cast< _Return( _Generic::* )( _Args... ) const >( &_Generic::operator() );
				}

				else if constexpr ( has_mutable_noexcept_lambda_op_v< _Generic, _Return( _Args... ) > )
				{
					return static_cast< _Return( _Generic::* )( _Args... ) noexcept >( &_Generic::operator() );
				}

				else if constexpr ( has_noexcept_lambda_op_v< _Generic, _Return( _Args... ) > )
				{
					return static_cast< _Return( _Generic::* )( _Args... ) const noexcept >( &_Generic::operator() );
				}

				else
				{
					return nullptr;
				}
			}( );

		using type = decltype( value );
	};

	template < typename _Generic, typename _Signature = std::conditional_t< is_lambda_v< _Generic >, get_fn_sig_t< _Generic >, void > >
	using get_lambda_op_t = typename get_lambda_op< _Generic, _Signature >::type;

	template < typename _Generic, typename _Signature = std::conditional_t< is_lambda_v< _Generic >, get_fn_sig_t< _Generic >, void > >
	static constexpr auto get_lambda_op_v = get_lambda_op< _Generic, _Signature >::value;

	template < typename... _Fns >
	struct overload_set : std::remove_reference_t< _Fns >... { explicit overload_set( _Fns&&... a_Fns ) : std::remove_reference_t< _Fns >( a_Fns )... {} using std::remove_reference_t< _Fns >::operator()...; };

	template < typename... _Fns >
	auto make_overload_set( _Fns&&... a_Fns )
	{
		return overload_set< _Fns... >( std::forward< _Fns >( a_Fns )... );
	}

	template < typename _Fn >
	struct fn
	{
		using type = _Fn;
		using traits_type = function_traits< type >;
		using return_type = get_fn_return_t< type >;
		using args_type = get_fn_args_t< type >;
		using sig_type = get_fn_sig_t< type >;
	};

	template < auto _Fn >
	struct member_fn : fn< decltype( _Fn ) >
	{
		static_assert( apl::is_member_fn_v< typename fn< decltype( _Fn ) >::type >, "Only member functions can be stored in a member_fn wrapper." );

		using object_type = get_fn_object_t< typename fn< decltype( _Fn ) >::type >;
		static constexpr auto value = _Fn;
	};

	template < auto _Fn >
	struct static_fn : fn< decltype( _Fn ) >
	{
		static_assert( apl::is_static_fn_v< decltype( _Fn ) >, "Only static functions can be stored in StaticFunction." );

		static constexpr auto value = _Fn;
	};
}