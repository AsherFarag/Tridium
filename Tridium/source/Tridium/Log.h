#pragma once

#include "Core/Core.h"
#include "spdlog/spdlog.h"

namespace Tridium {

	class TRIDIUM_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// - Core Log Macros -
#define TRI_CORE_TRACE(...) ::Tridium::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TRI_CORE_INFO(...)  ::Tridium::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TRI_CORE_WARN(...)  ::Tridium::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TRI_CORE_ERROR(...) ::Tridium::Log::GetCoreLogger()->error(__VA_ARGS__)
#define TRI_CORE_FATAL(...) ::Tridium::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// - Client Log Macros - 
#define LOG_TRACE(...)      ::Tridium::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)       ::Tridium::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)       ::Tridium::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)      ::Tridium::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...)      ::Tridium::Log::GetClientLogger()->fatal(__VA_ARGS__)