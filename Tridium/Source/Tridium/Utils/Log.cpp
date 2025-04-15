#include "tripch.h"
#include "Log.h"


namespace Tridium {

	UnorderedMap<hash_t, Log::Logger> Log::s_Loggers;
	SharedPtr<spdlog::sinks::stdout_color_sink_mt> Log::s_ColorSink;

	void Log::Init()
	{
		// Create a color sink
		s_ColorSink = MakeShared<spdlog::sinks::stdout_color_sink_mt>();
		s_ColorSink->set_pattern( "%^%T [%n] %v%$" );
	}

	Log::Logger& Log::GetLogger( const Category& a_Category )
	{
		auto it = s_Loggers.find( a_Category.Hash );
		if ( it != s_Loggers.end() )
			return it->second;

		auto spdlogger = MakeShared<spdlog::logger>( a_Category.Name, s_ColorSink );
		spdlogger->set_level( spdlog::level::trace );
		Log::Logger logger;
		logger.LogCategory = a_Category;
		logger.MessageLogger = std::move( spdlogger );
		return s_Loggers.emplace( a_Category.Hash, std::move( logger ) ).first->second;
	}
}