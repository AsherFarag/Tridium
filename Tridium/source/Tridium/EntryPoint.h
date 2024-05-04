#pragma once

#ifdef TRI_PLATFORM_WINDOWS

extern Tridium::Application* Tridium::CreateApplication();

int main(int argc, char** argv)
{ 
	Tridium::Log::Init();
	TRI_CORE_WARN( "Initialised Log!" );

	auto app = Tridium::CreateApplication();
	app->Run();
	delete app;
}

#endif // TRI_PLATFORM_WINDOWS
