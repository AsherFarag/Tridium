#include "tripch.h"
#include "Assert.h"
#include <Tridium/Utils/Log.h>

namespace Tridium::Internal {

    void Core_Message( const char* a_Type, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString = nullptr, va_list a_Inputs = nullptr )
    {
		if ( a_FormatString )
		{
			thread_local char formattedLog[1024];
			vsprintf_s( formattedLog, sizeof(formattedLog), a_FormatString, a_Inputs );
			TE_CORE_ERROR( "{0}: {1} ({2}:{3}) - MSG: {4}", a_Type, a_Function, a_File, a_Line, formattedLog );
		}
		else
		{
			TE_CORE_ERROR( "{0}: {1} ({2}:{3})", a_Type, a_Function, a_File, a_Line );
		}
    }

	void Message( const char* a_Type, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString = nullptr, va_list a_Inputs = nullptr )
	{
		if ( a_FormatString )
		{
			thread_local char formattedLog[1024];
			vsprintf_s( formattedLog, sizeof( formattedLog ), a_FormatString, a_Inputs );
			LOG_ERROR( "{0}: {1} ({2}:{3}) - MSG: {4}", a_Type, a_Function, a_File, a_Line, formattedLog );
		}
		else
		{
			LOG_ERROR( "{0}: {1} ({2}:{3})", a_Type, a_Function, a_File, a_Line );
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

#if CHECKS_ENABLED

	void Core_Check( bool a_Condition, const char* a_Function, const char* a_File, int a_Line )
	{
		if ( !a_Condition )
		{
			Core_Message( "CHECK FAILED", a_Function, a_File, a_Line );
		}
	}

	void Core_Check_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... )
	{
		if ( !a_Condition )
		{
			va_list Inputs;
			va_start( Inputs, a_FormatString );
			Message( "CHECK FAILED", a_Function, a_File, a_Line, a_FormatString, Inputs );
			va_end( Inputs );
		}
	}

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

#if ASSERTS_ENABLED

	bool Core_Assert( bool a_Condition, const char* a_Function, const char* a_File, int a_Line )
	{
		if ( !a_Condition )
		{
			Core_Message( "ASSERT FAILED", a_Function, a_File, a_Line );
		#if BREAK_ON_ASSERT_FAIL
			Break();
		#endif
		}

		return a_Condition;
	}

	bool Core_Assert_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... )
	{
		if ( !a_Condition )
		{
			va_list Inputs;
			va_start( Inputs, a_FormatString );
			Core_Message( "ASSERT FAILED", a_Function, a_File, a_Line, a_FormatString, Inputs );
			va_end( Inputs );
		#if BREAK_ON_ASSERT_FAIL
			Break();
		#endif
		}

		return a_Condition;
	}

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

#if ENSURES_ENABLED

	bool Core_Ensure( bool a_Condition, const char* a_Function, const char* a_File, int a_Line )
	{
		if ( !a_Condition )
		{
			Core_Message( "ENSURE FAILED", a_Function, a_File, a_Line );
			Abort();
		}

		return a_Condition;
	}

	bool Core_Ensure_Log( bool a_Condition, const char* a_Function, const char* a_File, int a_Line, const char* a_FormatString, ... )
	{
		if ( !a_Condition )
		{
			va_list Inputs;
			va_start( Inputs, a_FormatString );
			Core_Message( "ENSURE FAILED", a_Function, a_File, a_Line, a_FormatString, Inputs );
			va_end( Inputs );
			Abort();
		}

		return a_Condition;
	}

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

}