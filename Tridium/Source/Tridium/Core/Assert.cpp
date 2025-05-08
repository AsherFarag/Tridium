#include "tripch.h"
#include "Assert.h"
#include <Tridium/Utils/Log.h>

namespace Tridium::Debug {

	void Detail::AssertMessage( StringView a_Message )
	{
		LOG( LogCategory::Assert, Error, a_Message );
	}

#if NOT_IMPLEMENTED_ENABLED
	void NotImplemented( const char* a_Function, const char* a_File, int a_Line )
	{
		//Message( "NOT IMPLEMENTED", a_Function, a_File, a_Line );
		//Abort();
	}
#endif

}
