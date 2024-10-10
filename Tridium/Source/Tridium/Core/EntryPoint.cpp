#include "tripch.h"
#include <Tridium/ECS/Components/Types/Common/TestComponent.h>

#ifdef TE_PLATFORM_WINDOWS
#include <Tridium/Utils/Reflection/Reflection.h>

int main( int argc, char** argv )
{
	Tridium::Refl::__Internal_InitializeReflection();

	Tridium::Log::Init();
	TE_CORE_INFO( " - Initialised Log - " );

	TE_CORE_TRACE( "TRACE" );
	TE_CORE_DEBUG( "DEBUG" );
	TE_CORE_INFO( "INFO" );
	TE_CORE_WARN( "WARN" );
	TE_CORE_ERROR( "ERROR" );
	TE_CORE_FATAL( "FATAL" );

	TE_CORE_INFO( argc );
	for ( size_t i = 0; i < argc; ++i )
		TE_CORE_INFO( argv[i] );

	Tridium::Application app;
	app.Run();
}

#endif // TE_PLATFORM_WINDOWS