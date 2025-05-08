#pragma once
#include "Config.h"
#include <Tridium/Utils/Macro.h>
#include <format>
#include <Tridium/Containers/String.h>

namespace Tridium::Debug {

	// Helpers
	namespace Detail {

		void AssertMessage( StringView a_Message );

		template<typename... _Args>
		inline String FormatMsg( StringView a_Fmt, const _Args&... a_Args )
		{
			return std::vformat( a_Fmt, std::make_format_args( std::forward<const _Args>( a_Args )... ) );
		}

		inline String FormatMsg()
		{
			return {};
		}

		#define _ASSERT_MSG_FORMAT_WRAPPER( _Type ) \
			_Type " Failed: '{}', Where: Line - " TOSTRING( __LINE__ ) ", File - " TRIDIUM_FILE ", Function - " TRIDIUM_FUNCTION
		
		#define _DO_ONCE_WRAPPER( _Condition, _AssertMacro, _HasDoneStorage ) \
			([]( bool a_Condition ) { \
				_HasDoneStorage bool s_HasDone = false; if (!s_HasDone) { _AssertMacro; s_HasDone |= !a_Condition; } return a_Condition; \
			}( _Condition ))
	}



#if CONFIG_PLATFORM_WINDOWS
	#define DEBUG_BREAK() __debugbreak()
	#define ABORT() abort()
#else
	#define DEBUG_BREAK() No Debug Break for this platform
	#define ABORT() No Abort for this platform
#endif // CONFIG_PLATFORM_WINDOWS



///////////////////////////////////////////////////////////////////////////////////////////
// CHECK
//	Checks are used to verify conditions that shouldn't happen, but are not critical to the program.
// 	By default, checks are enabled in all builds except shipping builds.
// 	Checks also return the passed condition, so they can be used as a predicate in an if statement.
// 	Example: if ( CHECK( x ) ) { ... }
// 			 if ( CHECK( false ) ) - is the equivalent to - if ( false )
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_CHECKS_ENABLED
	#define CONFIG_CHECKS_ENABLED !CONFIG_SHIPPING
#endif // !CONFIG_CHECKS_ENABLED

	template<typename... _Args>
	bool Check( bool a_Condition, StringView a_Fmt, const _Args&... a_Args )
	{
	#if CONFIG_CHECKS_ENABLED
		if ( !a_Condition && !a_Fmt.empty() )
			Detail::AssertMessage( Detail::FormatMsg( a_Fmt, std::forward<const _Args>( a_Args )... ) );
	#endif // CONFIG_CHECKS_ENABLED

		return a_Condition;
	}

#if CONFIG_CHECKS_ENABLED
	// Checks that the condition is true, if not, it logs that the check failed at the source location.
	#define CHECK(_Condition, ...) \
		(::Tridium::Debug::Check(static_cast<bool>(_Condition), _ASSERT_MSG_FORMAT_WRAPPER("CHECK"), ::Tridium::Debug::Detail::FormatMsg( __VA_ARGS__ ) ))

	// Same as CHECK, but will only check once among all threads.
	#define CHECK_ONCE(_Condition, ...) \
		_DO_ONCE_WRAPPER( _Condition, CHECK(_Condition, __VA_ARGS__), static )

	// Same as CHECK, but will only check once among per thread.
	#define CHECK_ONCE_PER_THREAD(_Condition, ...) \
		_DO_ONCE_WRAPPER( _Condition, CHECK(_Condition, __VA_ARGS__), thread_local )

#else
	#define CHECK(_Condition, ...)
	#define CHECK_ONCE(_Condition, ...)
	#define CHECK_ONCE_PER_THREAD(_Condition, ...)
#endif // CONFIG_CHECKS_ENABLED



///////////////////////////////////////////////////////////////////////////////////////////
// ASSERTS
// 	Asserts are used to verify conditions that shouldn't happen and will cause a debug break if the assert fails.
// 	By default, ASSERT's are enabled in all builds except shipping builds.
// 	Asserts also return the passed condition, so they can be used as a predicate in an if statement.
//  Example: if ( ASSERT( x ) ) { ... }
// 	  		 if ( ASSERT( false ) ) - is the equivalent to - if ( false )
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_ASSERTS_ENABLED
	#define CONFIG_ASSERTS_ENABLED !CONFIG_SHIPPING
#endif // !CONFIG_ASSERTS_ENABLED

	template<typename... _Args>
	bool Assert( bool a_Condition, StringView a_Fmt = {}, const _Args&... a_Args )
	{
	#if CONFIG_ASSERTS_ENABLED
		if ( !a_Condition )
		{
			if ( !a_Fmt.empty() )
				Detail::AssertMessage( Detail::FormatMsg( a_Fmt, std::forward<const _Args>( a_Args )... ) );

			DEBUG_BREAK();
		}
	#endif // CONFIG_ASSERTS_ENABLED

		return a_Condition;
	}

#if CONFIG_ASSERTS_ENABLED

	// Asserts that the condition is true, if not, it logs that the assert failed at the source location and breaks the program.
	#define ASSERT(_Condition, ...) \
		(::Tridium::Debug::Assert(static_cast<bool>(_Condition), _ASSERT_MSG_FORMAT_WRAPPER("ASSERT"), ::Tridium::Debug::Detail::FormatMsg( __VA_ARGS__ ) ))

	// Same as ASSERT, but will only assert once among all threads.
	#define ASSERT_ONCE(_Condition, ...) \
		_DO_ONCE_WRAPPER( _Condition, ASSERT(_Condition, __VA_ARGS__), static )

	// Same as ASSERT, but will only assert once among per thread.
	#define ASSERT_ONCE_PER_THREAD(_Condition, ...) \
		_DO_ONCE_WRAPPER( _Condition, ASSERT(_Condition, __VA_ARGS__), thread_local )


#else
	#define ASSERT(_Condition, ...)
	#define ASSERT_ONCE(_Condition, ...)
	#define ASSERT_ONCE_PER_THREAD(_Condition, ...)
#endif // CONFIG_ASSERTS_ENABLED



///////////////////////////////////////////////////////////////////////////////////////////
// ENSURES
//  Ensures are used to verify conditions that, if invalid, can be fatal to the program.
// 	By default, ENSURE's are always enabled.
//  They will always log the failure and then abort the program.
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_ENSURES_ENABLED 
	#define CONFIG_ENSURES_ENABLED 1
#endif // CONFIG_ENSURES_ENABLED


	template<typename... _Args>
	void Ensure( bool a_Condition, StringView a_Fmt = {}, const _Args&... a_Args )
	{
	#if CONFIG_ENSURES_ENABLED
		if ( !a_Condition )
		{
			if ( !a_Fmt.empty() )
				Detail::AssertMessage( Detail::FormatMsg( a_Fmt, std::forward<const _Args>( a_Args )... ) );

			ABORT();
		}
	#endif // CONFIG_ENSURES_ENABLED
	}

#if CONFIG_ENSURES_ENABLED
	#define ENSURE(_Condition, ...) \
		(::Tridium::Debug::Ensure(static_cast<bool>(_Condition), _ASSERT_MSG_FORMAT_WRAPPER("ENSURE"), ::Tridium::Debug::Detail::FormatMsg( __VA_ARGS__ ) ))
#else
	#define ENSURE(_Condition, ...)
#endif // CONFIG_ENSURES_ENABLED

///////////////////////////////////////////////////////////////////////////////////////////
// NOT IMPLEMENTED
///////////////////////////////////////////////////////////////////////////////////////////

#define NOT_IMPLEMENTED_STATIC PRAGMA( message( TRIDIUM_FILE "(" $LINE ") : Not Implemented" ) )

#ifndef NOT_IMPLEMENTED_ENABLED
	#define NOT_IMPLEMENTED_ENABLED 1
#endif // NOT_IMPLEMENTED_ENABLED

#if NOT_IMPLEMENTED_ENABLED
	#define NOT_IMPLEMENTED \
		NOT_IMPLEMENTED_STATIC; \
		( ::Tridium::Debug::NotImplemented( TRIDIUM_FUNCTION, TRIDIUM_FILE, TRIDIUM_LINE ) )

	void NotImplemented( const char* a_Function, const char* a_File, int a_Line );
#else
	#define NOT_IMPLEMENTED()
#endif // NOT_IMPLEMENTED_ENABLED

} // namespace Tridium