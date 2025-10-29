#include "PlatformInterface.h"

namespace Tridium {

#if CONFIG_PLATFORM_WINDOWS || CONFIG_PLATFORM_LINUX || CONFIG_PLATFORM_MACOS
	extern UniquePtr<IPlatformInterface> CreatePlatformInterface_GLFW();
#endif // CONFIG_PLATFORM_WINDOWS || CONFIG_PLATFORM_LINUX || CONFIG_PLATFORM_MACOS

	UniquePtr<IPlatformInterface> IPlatformInterface::Create( const PlatformSettings& a_PlatformSettings )
	{
		UniquePtr<IPlatformInterface> platformInterface;

	#if CONFIG_PLATFORM_WINDOWS || CONFIG_PLATFORM_LINUX || CONFIG_PLATFORM_MACOS
		platformInterface = CreatePlatformInterface_GLFW();
	#else
		#error "Platform interface not implemented for this platform!"
		NOT_IMPLEMENTED;
		return nullptr;
	#endif // CONFIG_PLATFORM_WINDOWS || CONFIG_PLATFORM_LINUX || CONFIG_PLATFORM_MACOS

		bool success = platformInterface->Setup( a_PlatformSettings );
		return platformInterface;
	}

} // namespace Tridium