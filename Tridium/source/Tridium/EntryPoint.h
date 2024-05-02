#pragma once

#ifdef TRI_PLATFORM_WINDOWS

extern Tridium::Application* Tridium::CreateApplication();

int main(int argc, char** argv)
{ 
	auto app = Tridium::CreateApplication();
	app->Run();
	delete app;
}

#endif // TRI_PLATFORM_WINDOWS
