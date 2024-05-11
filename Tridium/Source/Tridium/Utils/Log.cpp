#include "tripch.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Tridium {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern( "%^[%T] %n: %v%$" );
		// Core Logger
		s_CoreLogger = spdlog::stdout_color_mt("TRIDIUM");
		s_CoreLogger->set_level( spdlog::level::trace );
		// Client Logger
		s_ClientLogger = spdlog::stdout_color_mt( "APP" );
		s_ClientLogger->set_level( spdlog::level::trace );
	}
}