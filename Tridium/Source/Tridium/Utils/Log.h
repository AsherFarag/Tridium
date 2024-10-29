#pragma once

#include "Tridium/Core/Core.h"
#include "spdlog/spdlog.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

namespace Tridium {

	class Log
	{
	public:
		static void Init();

		inline static SharedPtr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static SharedPtr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static SharedPtr<spdlog::logger> s_CoreLogger;
		static SharedPtr<spdlog::logger> s_ClientLogger;
	};

}

//template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
//inline OStream& operator<<( OStream& os, const glm::vec<L, T, Q>& vector )
//{
//	return os << glm::to_string( vector );
//}
//
//template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
//inline OStream& operator<<( OStream& os, const glm::mat<C, R, T, Q>& matrix )
//{
//	return os << glm::to_string( matrix );
//}
//
//template<typename OStream, typename T, glm::qualifier Q>
//inline OStream& operator<<( OStream& os, glm::qua<T, Q> quaternion )
//{
//	return os << glm::to_string( quaternion );
//}

#define TE_USE_LOGGING 1
#if TE_USE_LOGGING

// - Core Log Macros -
#define TE_CORE_TRACE(...)	::Tridium::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TE_CORE_DEBUG(...)	::Tridium::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define TE_CORE_INFO(...)	::Tridium::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TE_CORE_WARN(...)	::Tridium::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TE_CORE_ERROR(...)	::Tridium::Log::GetCoreLogger()->error(__VA_ARGS__)
#define TE_CORE_FATAL(...)	::Tridium::Log::GetCoreLogger()->critical(__VA_ARGS__)

// - Client Log Macros - 
#define LOG_TRACE(...)      ::Tridium::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)		::Tridium::Log::GetClientLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)       ::Tridium::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)       ::Tridium::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)      ::Tridium::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...)      ::Tridium::Log::GetClientLogger()->critical(__VA_ARGS__)

#else

// - Core Log Macros -
#define TE_CORE_TRACE(...)	
#define TE_CORE_DEBUG(...)	
#define TE_CORE_INFO(...)	
#define TE_CORE_WARN(...)	
#define TE_CORE_ERROR(...)	
#define TE_CORE_FATAL(...)	

// - Client Log Macros - 
#define LOG_TRACE(...)      
#define LOG_DEBUG(...)		
#define LOG_INFO(...)       
#define LOG_WARN(...)       
#define LOG_ERROR(...)      
#define LOG_FATAL(...)      

#endif