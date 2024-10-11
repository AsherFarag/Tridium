#include "tripch.h"
#include <Tridium/ECS/Components/Types/Common/TestComponent.h>

#ifdef TE_PLATFORM_WINDOWS
#include <Tridium/Utils/Reflection/Reflection.h>
#include <fstream>

int main( int argc, char** argv )
{
	Tridium::Refl::__Internal_InitializeReflection();

	Tridium::Log::Init();

	Tridium::Application app;
	app.Run();
}

#endif // TE_PLATFORM_WINDOWS