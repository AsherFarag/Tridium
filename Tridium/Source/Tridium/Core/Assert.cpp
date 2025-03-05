#include "tripch.h"
#include "Assert.h"
#include <Tridium/Utils/Log.h>

namespace Tridium::Internal {

	void Message( const char* a_Type, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString = nullptr, va_list a_Inputs = nullptr )
	{
		if ( a_FormatString )
		{
			thread_local char formattedLog[1024];
			vsprintf_s( formattedLog, sizeof( formattedLog ), a_FormatString, a_Inputs );
			LOG( LogCategory::Assert, Error, "{0}: {4} - Where: {1} ({2}:{3})", a_Type, a_Function, a_File, a_Line, formattedLog );
		}
		else
		{
			LOG( LogCategory::Assert, Error, "{0}: Where: {1} ({2}:{3})", a_Type, a_Function, a_File, a_Line );
		}
	}

	void Break()
	{
		__debugbreak();
	}

	void Abort()
	{
		abort();
	}

#if CONFIG_CHECKS_ENABLED

	void Check( bool a_Condition, const char* a_Function, const char* a_File, int a_Line )
	{
		if ( !a_Condition )
		{
			Message( "CHECK FAILED", a_Function, a_File, a_Line );
		}
	}

	void Check_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... )
	{
		if ( !a_Condition )
		{
			va_list Inputs;
			va_start( Inputs, a_FormatString );
			Message( "CHECK FAILED", a_Function, a_File, a_Line, a_FormatString, Inputs );
			va_end( Inputs );
		}
	}

#endif

#if CONFIG_ASSERTS_ENABLED

	bool Assert( bool a_Condition, const char* a_Function, const char* a_File, int a_Line )
	{
		if ( !a_Condition )
		{
			Message( "ASSERT FAILED", a_Function, a_File, a_Line );
		#if BREAK_ON_ASSERT_FAIL
			Break();
		#endif
		}

		return a_Condition;
	}

	bool Assert_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... )
	{
		if ( !a_Condition )
		{
			va_list Inputs;
			va_start( Inputs, a_FormatString );
			Message( "ASSERT FAILED", a_Function, a_File, a_Line, a_FormatString, Inputs );
			va_end( Inputs );
		#if BREAK_ON_ASSERT_FAIL
			Break();
		#endif
		}

		return a_Condition;
	}

#endif

#if CONFIG_ENSURES_ENABLED

	bool Ensure( bool a_Condition, const char* a_Function, const char* a_File, int a_Line )
	{
		if ( !a_Condition )
		{
			Message( "ENSURE FAILED", a_Function, a_File, a_Line );
			Abort();
		}
		
		return !a_Condition;
	}

	bool Ensure_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... )
	{
		if ( !a_Condition )
		{
			va_list Inputs;
			va_start( Inputs, a_FormatString );
			Message( "ENSURE FAILED", a_Function, a_File, a_Line, a_FormatString, Inputs );
			va_end( Inputs );
			Abort();
		}

		return a_Condition;
	}

#endif

#if NOT_IMPLEMENTED_ENABLED
	void NotImplemented( const char* a_Function, const char* a_File, int a_Line )
	{
		Message( "NOT IMPLEMENTED", a_Function, a_File, a_Line );
		Abort();
	}
#endif

}