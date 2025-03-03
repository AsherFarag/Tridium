#pragma once
#include "Config.h"
#include <Tridium/Utils/Macro.h>

// Use of namespace to hide function names from the global namespace
namespace Tridium::Internal {

///////////////////////////////////////////////////////////////////////////////////////////
// CHECKS
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_CHECKS_ENABLED
	#define CONFIG_CHECKS_ENABLED !CONFIG_SHIPPING
#endif // !CONFIG_CHECKS_ENABLED

#if CONFIG_CHECKS_ENABLED
	// Checks if the condition is true, if not, it logs that the check failed at the source location.
	// Note: This does not break the program, it only logs the check failure.
	#define CHECK(x) (::Tridium::Internal::Check(static_cast<bool>(x), TRIDIUM_FUNCTION, TRIDIUM_FILE, TRIDIUM_LINE))

	// Checks if the condition is true, if not, it logs that the check failed at the source location with a custom message.
	// Note: This does not break the program, it only logs the check failure.
	#define CHECK_LOG(x, ...) (::Tridium::Internal::Check_Log(static_cast<bool>(x), TRIDIUM_FUNCTION, TRIDIUM_FILE, TRIDIUM_LINE, __VA_ARGS__))

	void Check( bool a_Condition, const char* a_Function, const char* a_File, int a_Line );
	void Check_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... );
#else
	#define CHECK(x)
	#define CHECK_LOG(x, ...)
#endif // CONFIG_CHECKS_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////
// ASSERTS
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_ASSERTS_ENABLED
	#define CONFIG_ASSERTS_ENABLED !CONFIG_SHIPPING
#endif // !CONFIG_ASSERTS_ENABLED

#if CONFIG_ASSERTS_ENABLED
	#define BREAK_ON_ASSERT_FAIL 1

	// Asserts if the condition is false, if so, it logs that the assert failed at the source location and breaks.
	// Can be used as a predicate in an if statement. Example: if ( ASSERT( x ) ) { ... }
	// Note: The boolean output is the same as the input condition. So, if the condition is false, the assert will break and return false.
	#define ASSERT(x) (::Tridium::Internal::Assert(static_cast<bool>(x), TRIDIUM_FUNCTION, TRIDIUM_FILE, TRIDIUM_LINE))

	// Asserts if the condition is false, if so, it logs that the assert failed at the source location with a custom message and breaks.
	// Can be used as a predicate in an if statement. Example: if ( ASSERT_LOG( x, "Message" ) ) { ... }
	// Note: The boolean output is the same as the input condition. So, if the condition is false, the assert will break and return false.
	#define ASSERT_LOG(x, ...) (::Tridium::Internal::Assert_Log(static_cast<bool>(x), TRIDIUM_FUNCTION, TRIDIUM_FILE, TRIDIUM_LINE, __VA_ARGS__))

	// Works the same as ASSERT, but only asserts once. If the condition is false, it will assert and then never assert again.
	#define ASSERT_ONCE(x) do { static bool s_HasAsserted = false; if ( !s_HasAsserted && !(x) ) { s_HasAsserted = true; ASSERT( x ); } } while ( false )

	// Works the same as ASSERT_LOG, but only asserts once. If the condition is false, it will assert and then never assert again.
	#define ASSERT_ONCE_LOG(x, ...) do { static bool s_HasAsserted = false; if ( !s_HasAsserted && !(x) ) { s_HasAsserted = true; ASSERT_LOG( x, __VA_ARGS__ ); } } while ( false )

	// Works the same as ASSERT, but only asserts once per thread. If the condition is false, it will assert and then never assert again in that thread.
	#define ASSERT_ONCE_PER_THREAD(x) do { thread_local bool s_HasAsserted = false; if ( !s_HasAsserted && !(x) ) { s_HasAsserted = true; ASSERT( x ); } } while ( false )

	// Works the same as ASSERT_LOG, but only asserts once per thread. If the condition is false, it will assert and then never assert again in that thread.
	#define ASSERT_ONCE_PER_THREAD_LOG(x, ...) do { thread_local bool s_HasAsserted = false; if ( !s_HasAsserted && !(x) ) { s_HasAsserted = true; ASSERT_LOG( x, __VA_ARGS__ ); } } while ( false )

	bool Assert( bool a_Condition, const char* a_Function, const char* a_File, int a_Line );
	bool Assert_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... );
#else
	#define BREAK_ON_ASSERT_FAIL 0
	#define ASSERT(x) (x)
	#define ASSERT_LOG(x, ...) (x)
	#define ASSERT_ONCE(x) (x)
	#define ASSERT_ONCE_LOG(x, ...) (x)
	#define ASSERT_ONCE_PER_THREAD(x) (x)
	#define ASSERT_ONCE_PER_THREAD_LOG(x, ...) (x)
#endif // CONFIG_ASSERTS_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////
// ENSURES
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_ENSURES_ENABLED 
	#define CONFIG_ENSURES_ENABLED ( CONFIG_DEBUG || CONFIG_SHIPPING )
#endif // CONFIG_ENSURES_ENABLED

#if CONFIG_ENSURES_ENABLED
	// Ensures that the condition is true, if not, it logs that the ensure failed at the source location and breaks the program, even in shipping builds.
	// Use this for conditions that should never fail and can cause serious issues if they do.
	#define ENSURE(x) (::Tridium::Internal::Ensure(static_cast<bool>(x), TRIDIUM_FUNCTION, TRIDIUM_FILE, TRIDIUM_LINE))

	// Ensures that the condition is true, if not, it logs that the ensure failed at the source location with a custom message and breaks the program, even in shipping builds.
	// Use this for conditions that should never fail and can cause serious issues if they do.
	#define ENSURE_LOG(x, ...) (::Tridium::Internal::Ensure_Log(static_cast<bool>(x), TRIDIUM_FUNCTION, TRIDIUM_FILE, TRIDIUM_LINE, __VA_ARGS__))


	bool Ensure( bool a_Condition, const char* a_Function, const char* a_File, int a_Line );
	bool Ensure_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... );
#else
	#define ENSURE(x) (x)
	#define ENSURE_LOG(x, ...) (x)
#endif // CONFIG_ENSURES_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////
// NOT IMPLEMENTED
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef NOT_IMPLEMENTED_ENABLED
	#define NOT_IMPLEMENTED_ENABLED 1
#endif // NOT_IMPLEMENTED_ENABLED

#if NOT_IMPLEMENTED_ENABLED
	#define NOT_IMPLEMENTED \
		PRAGMA( message( TRIDIUM_FILE "(" $LINE ") : Not Implemented" ) ); \
		( ::Tridium::Internal::NotImplemented( TRIDIUM_FUNCTION, TRIDIUM_FILE, TRIDIUM_LINE ) )

	void NotImplemented( const char* a_Function, const char* a_File, int a_Line );
#else
	#define NOT_IMPLEMENTED()
#endif // NOT_IMPLEMENTED_ENABLED

} // namespace Tridium