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

	const char* ProjectPath = argc >= 2 ? argv[1] : "";

	TE_CORE_INFO( "Starting Tridium Engine" );
	TE_CORE_INFO( "Project Path: {0}", ProjectPath );

	Tridium::Application app(ProjectPath);
	app.Run();
}

#endif // TE_PLATFORM_WINDOWS