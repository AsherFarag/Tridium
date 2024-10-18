#include "tripch.h"
#ifdef TE_PLATFORM_WINDOWS
#include <Tridium/Core/Application.h>
#include <Tridium/Utils/Reflection/Reflection.h>

int main( int argc, char** argv )
{
	Tridium::Refl::__Internal_InitializeReflection();
	Tridium::Log::Init();

	const char* ProjectPath = argc >= 2 ? argv[1] : "";

	Tridium::Application app(ProjectPath);
	app.Run();
}

#endif // TE_PLATFORM_WINDOWS