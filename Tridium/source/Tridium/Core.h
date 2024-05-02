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
