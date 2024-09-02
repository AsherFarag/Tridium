#pragma once

#ifdef TE_PLATFORM_WINDOWS

extern Tridium::Application* Tridium::CreateApplication();

int main(int argc, char** argv)
{ 
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

	auto app = Tridium::CreateApplication();
	app->Run();
	delete app;
}

#endif // TE_PLATFORM_WINDOWS
