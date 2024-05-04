#pragma once

#ifdef TRI_PLATFORM_WINDOWS
	#ifdef TRI_BUILD_DLL
		#define TRIDIUM_API __declspec(dllexport)
	#else
		#define TRIDIUM_API __declspec(dllimport)
	#endif // TRI_BUILD_DLL
#else
	#error Tridium currently only supports Windows!
#endif // TRI_PLATFORM_WINDOWS

#ifdef TRI_ENABLE_ASSERTS
	#define TRI_ASSERT(x, ...) { if (!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define TRI_CORE_ASSERT(x, ...) { if (!(x)) { TRI_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define TRI_ASSERT(x, ...)
	#define TRI_CORE_ASSERT(x, ...)
#endif // TRI_ENABLE_ASSERTS


#define BIT(x) (1 << x)