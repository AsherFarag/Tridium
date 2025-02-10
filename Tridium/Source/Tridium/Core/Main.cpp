#include "tripch.h"
#ifdef TE_PLATFORM_WINDOWS
#include <Tridium/Core/Application.h>
#include <Tridium/Reflection/PrimitiveReflector.h>
#include <Tridium/Reflection/ReflectorInitializer.h>

int main( int argc, char** argv )
{
	Tridium::Log::Init();

	Tridium::Refl::Internal::ReflectPrimitiveTypes();
	Tridium::Refl::Internal::HiddenTypeReflector::ReflectHiddenTypes();

	Tridium::CmdLineArgs cmdLineArgs;
	cmdLineArgs.Args.Reserve( argc );
	for ( int i = 0; i < argc; ++i )
	{
		cmdLineArgs.Args.EmplaceBack( argv[i] );
	}

	Tridium::Application app( cmdLineArgs );
	app.Run();

#if CONFIG_USE_EDITOR
	// Wait for a key press before closing the console
	std::cin.get();
#endif // CONFIG_USE_EDITOR

	return 0;
}

#endif // TE_PLATFORM_WINDOWS