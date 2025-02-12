#pragma once

#include <Tridium/Core/Config.h>
#include <Tridium/Core/Types.h>
#include <Tridium/Containers/Map.h>
#include <Tridium/Containers/String.h>
#include <Tridium/Core/Memory.h>
#include "Macro.h"
#include <Tridium/Core/Hash.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Tridium {

	class Log
	{
	public:
		enum class ELevel : uint8_t
		{
			Trace = 0,
			Debug,
			Info,
			Warn,
			Error,
			Fatal
		};

		struct Category
		{
			const char* Name;
			hash_t Hash;

			constexpr Category()
				: Category( "Unknown" ) {}

			constexpr Category( const char* a_Name )
				: Name( a_Name )
				, Hash( Hashing::Hash( a_Name ) ) {}
		};

		struct Logger
		{
			Category LogCategory{ "Unknown" };
			SharedPtr<spdlog::logger> MessageLogger{ nullptr };
		};

		static void Init();
		static Logger& GetLogger( const Category& a_Category );

		static constexpr spdlog::level::level_enum LogLevelToSpdlogLevel( ELevel a_Level )
		{
			switch ( a_Level )
			{
			case ELevel::Trace: return spdlog::level::trace;
			case ELevel::Debug: return spdlog::level::debug;
			case ELevel::Info: return spdlog::level::info;
			case ELevel::Warn: return spdlog::level::warn;
			case ELevel::Error: return spdlog::level::err;
			case ELevel::Fatal: return spdlog::level::critical;
			default: return spdlog::level::info;
			}
		}

	private:
		static UnorderedMap<hash_t, Logger> s_Loggers;
		static SharedPtr<spdlog::sinks::stdout_color_sink_mt> s_ColorSink;
	};

}

#if CONFIG_USE_LOGGING
	#define LOG( _Category, _Level, ... ) do { ::Tridium::Log::GetLogger(_Category).MessageLogger->log( Log::LogLevelToSpdlogLevel(Log::ELevel::_Level), __VA_ARGS__ ); } while(0)
#else
	#define LOG( ... )
#endif

// Define a custom log category for use in the LOG macro
#define DECLARE_LOG_CATEGORY( _Name ) \
	namespace LogCategory { \
		static constexpr ::Tridium::Log::Category _Name = { #_Name }; \
	}

// Default log categories

DECLARE_LOG_CATEGORY( Default );
DECLARE_LOG_CATEGORY( Animation );
DECLARE_LOG_CATEGORY( AI );
DECLARE_LOG_CATEGORY( Application );
DECLARE_LOG_CATEGORY( Asset );
DECLARE_LOG_CATEGORY( Audio );
DECLARE_LOG_CATEGORY( Core );
DECLARE_LOG_CATEGORY( Debug );
DECLARE_LOG_CATEGORY( Editor );
DECLARE_LOG_CATEGORY( Engine );
DECLARE_LOG_CATEGORY( GameLogic );
DECLARE_LOG_CATEGORY( Input );
DECLARE_LOG_CATEGORY( IO );
DECLARE_LOG_CATEGORY( Network );
DECLARE_LOG_CATEGORY( Physics );
DECLARE_LOG_CATEGORY( Reflection );
DECLARE_LOG_CATEGORY( Rendering );
DECLARE_LOG_CATEGORY( Scene );
DECLARE_LOG_CATEGORY( Script );
DECLARE_LOG_CATEGORY( Serialization );
DECLARE_LOG_CATEGORY( UI );

// Assert Categories
DECLARE_LOG_CATEGORY( Check );
DECLARE_LOG_CATEGORY( Assert );
DECLARE_LOG_CATEGORY( Ensure );
