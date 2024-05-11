#pragma once

#ifdef TE_PLATFORM_WINDOWS

extern Tridium::Application* Tridium::CreateApplication();

int main(int argc, char** argv)
{ 
	Tridium::Log::Init();
	TE_CORE_WARN( "Initialised Log!" );

	auto app = Tridium::CreateApplication();
	app->Run();
	delete app;
}

#endif // TE_PLATFORM_WINDOWS
