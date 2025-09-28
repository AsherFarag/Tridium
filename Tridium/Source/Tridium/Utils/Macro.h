#pragma once

#define BIT(x) (1 << x)

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define STRINGIFY( x ) #x
#define TOSTRING( x ) STRINGIFY( x )
#define TO_LSTRING( x ) CONCAT(L, TOSTRING( x ) )
#define WRAP( m, x ) m(x)
#define PRAGMA(p) _Pragma(#p)
#define EXPAND(x) x

#pragma region Selectors

#define SELECT_MACRO_2(_1, _2, x, ...) x
#define SELECT_MACRO_3(_1, _2, _3, x, ...) x
#define SELECT_MACRO_4(_1, _2, _3, _4, x, ...) x
#define SELECT_MACRO_5(_1, _2, _3, _4, _5, x, ...) x
#define SELECT_MACRO_6(_1, _2, _3, _4, _5, _6, x, ...) x
#define SELECT_MACRO_7(_1, _2, _3, _4, _5, _6, _7, x, ...) x
#define SELECT_MACRO_8(_1, _2, _3, _4, _5, _6, _7, _8, x, ...) x
#define SELECT_MACRO_9(_1, _2, _3, _4, _5, _6, _7, _8, _9, x, ...) x
#define SELECT_MACRO_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, x, ...) x

#pragma endregion

// Internal helpers
#define _HAS_ARGS(...) _HAS_ARGS_IMPL(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define _HAS_ARGS_IMPL(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N

#define _COMMA_IF_ARGS_IMPL_1(...) , __VA_ARGS__
#define _COMMA_IF_ARGS_IMPL_0(...)

#define _COMMA_IF_ARGS_SELECT(N) _COMMA_IF_ARGS_IMPL_##N
#define _COMMA_IF_ARGS_IMPL(N, ...) _COMMA_IF_ARGS_SELECT(N)(__VA_ARGS__)

// Public macro
#define COMMA_IF_ARGS(...) EXPAND(_COMMA_IF_ARGS_IMPL(_HAS_ARGS(__VA_ARGS__), __VA_ARGS__))

#define TRIDIUM_NODISCARD [[nodiscard]]

#if defined( _MSC_VER )
	#define _FUNCTION_ EXPAND( __FUNCTION__ )
	#define _FUNCTION_SIG_ EXPAND( __FUNCSIG__ )
#elif defined(__clang__)
	#define _FUNCTION_ EXPAND( __PRETTY_FUNCTION__ )
	#define _FUNCTION_SIG_ EXPAND( __PRETTY_FUNCTION__ )
#endif

#if defined __clang__ || defined __GNUC__
	#define TRIDIUM_FILE __FILE__
	#define TRIDIUM_LINE uint32_t(_CONCAT(__LINE__,U))
	#define TRIDIUM_FUNCTION __FUNCTION__
	#define TRIDIUM_PRETTY_FUNCTION __PRETTY_FUNCTION__
	#define TRIDIUM_PRETTY_FUNCTION_PREFIX '='
	#define TRIDIUM_PRETTY_FUNCTION_SUFFIX ']'
#elif defined _MSC_VER
	#define TRIDIUM_FILE __FILE__
	#define TRIDIUM_LINE uint32_t(_CONCAT(__LINE__,U))
	#define TRIDIUM_FUNCTION __FUNCTION__
	#define TRIDIUM_PRETTY_FUNCTION __FUNCSIG__
	#define TRIDIUM_PRETTY_FUNCTION_PREFIX '<'
	#define TRIDIUM_PRETTY_FUNCTION_SUFFIX '>'
#endif

#ifdef _MSC_VER
	#define PACKED_STRUCT(_Declaration) __pragma(pack(push, 1)) _Declaration
	#define PACKED_STRUCT_END __pragma(pack(pop))
#else
	#define PACKED_STRUCT(_Declaration) _Declaration __attribute((packed))
	#define PACKED_STRUCT_END
#endif

#define $LINE WRAP( Stringize, __LINE__ )

// For some reason, in MSVC, __LINE__ is not a constant expression.
// This is a workaround to make it a constant expression.
#define _USABLE_LINE_ TRIDIUM_LINE

#if defined(_MSC_VER)
	#define FORCE_USE __declspec(selectany)
#elif defined(__GNUC__)
	#define FORCE_USE __attribute__((used))
#else
	#define FORCE_USE
#endif

#if defined(_MSC_VER)
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT
#endif

#define ALWAYS_EXISTS( _Variable ) DLL_EXPORT [[maybe_unused]] inline auto& CONCAT( __ForceExist_, _Variable )() { return _Variable; }