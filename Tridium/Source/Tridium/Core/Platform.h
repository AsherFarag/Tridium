#pragma once
#include "Config.h"

#if CONFIG_PLATFORM_WINDOWS

	#ifndef NOMINMAX
		#define NOMINMAX
	#endif // NOMINMAX

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif // WIN32_LEAN_AND_MEAN

	#include <Windows.h>

	#define Tridium_LoadLibrary( a_Path ) ::LoadLibraryA( a_Path )
	#define Tridium_GetProcAddress( a_Library, a_FunctionName ) ::GetProcAddress( Cast<HMODULE>( a_Library ), a_FunctionName )
	#define Tridium_FreeLibrary( a_Library ) ::FreeLibrary( Cast<HMODULE>( a_Library ) )

#elif CONFIG_PLATFORM_LINUX
	#include <dlfcn.h>

	#define Tridium_LoadLibrary( a_Path ) ::dlopen( a_Path, RTLD_NOW )
	#define Tridium_GetProcAddress( a_Library, a_FunctionName ) ::dlsym( a_Library, a_FunctionName )
	#define Tridium_FreeLibrary( a_Library ) ::dlclose( a_Library )

#else
	#error "Unsupported platform"
#endif // CONFIG_PLATFORM_LINUX

namespace Tridium {

	namespace Platform {

		using Handle = void*;
		using Module = Handle;

		// Loads a dynamic library (dll/so) from the specified path and returns a handle to it
		inline Module LoadDynamicLibrary( const char* a_Path )
		{
			return Tridium_LoadLibrary( a_Path );
		}

		// Get a function pointer from a dynamic library (dll/so) by name
		inline Handle GetProcAddress( Module a_Library, const char* a_FunctionName )
		{
			return Tridium_GetProcAddress( a_Library, a_FunctionName );
		}

		// Free a dynamic library
		inline void FreeDynamicLibrary( Module a_Library )
		{
			Tridium_FreeLibrary( a_Library );
		}

	} // namespace Platform

} // namespace Tridium