#include "tripch.h"
#ifdef TE_PLATFORM_WINDOWS
#include <Tridium/Core/Application.h>
#include <Tridium/Editor/Editor.h>
#include <Tridium/Reflection/PrimitiveReflector.h>
#include <Tridium/Reflection/ReflectorInitializer.h>

using namespace Tridium;

int main( int argc, char** argv )
{
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	Log::Init();

	//Refl::Internal::ReflectPrimitiveTypes();
	//Refl::Internal::HiddenTypeReflector::ReflectHiddenTypes();

	CmdLineArgs cmdLineArgs;
	cmdLineArgs.Args.Reserve( argc );
	for ( int i = 0; i < argc; ++i )
	{
		cmdLineArgs.Args.EmplaceBack( argv[i] );
	}

#if 0
	RHITest();
	return 0;
#endif

#if IS_EDITOR
	Editor app( std::move( cmdLineArgs ) );
	app.Run();
#else
	Application app( cmdLineArgs );
	app.Run();
#endif // IS_EDITOR

	return 0;
}

#endif // TE_PLATFORM_WINDOWS