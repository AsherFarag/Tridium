#include "tripch.h"
#include "Log.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Tridium {

	Ref<spdlog::logger> Log::s_CoreLogger;
	Ref<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		// Colours are swapped for some reason
		auto RED = FOREGROUND_BLUE;
		auto GREEN = FOREGROUND_GREEN;
		auto BLUE = FOREGROUND_RED;

		// Create a color sink
		auto color_sink = MakeRef<spdlog::sinks::stdout_color_sink_mt>();
		color_sink->set_pattern( "%^[%T] %n: %v%$" );
		// For some reason console colours on home machine are different
		//color_sink->set_color( spdlog::level::trace, RED | GREEN | BLUE ); // White
		//color_sink->set_color( spdlog::level::debug, GREEN | BLUE ); // Aqua
		//color_sink->set_color( spdlog::level::info, GREEN ); // Green
		//color_sink->set_color( spdlog::level::warn, BLUE ); // Somehow yellow
		//color_sink->set_color( spdlog::level::err, RED ); // Red
		//color_sink->set_color( spdlog::level::critical, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE ); // White Text, Red Background

		// Core Logger
		s_CoreLogger = MakeRef<spdlog::logger>( "TRIDIUM", color_sink );
		s_CoreLogger->set_level( spdlog::level::trace );

		// Client Logger
		s_ClientLogger = MakeRef<spdlog::logger>( "APP", color_sink );
		s_ClientLogger->set_level( spdlog::level::trace );
	}
}