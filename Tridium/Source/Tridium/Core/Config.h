#pragma once

//=================================================================================================
// Platform Configuration
#define CONFIG_PLATFORM_WINDOWS      1
#define CONFIG_PLATFORM_LINUX        0
#define CONFIG_PLATFORM_MACOS        0
#define CONFIG_PLATFORM_XBOX         0
#define CONFIG_PLATFORM_IOS		     0
#define CONFIG_PLATFORM_ANDROID      0
//=================================================================================================

#ifdef TE_SHIPPING
	#define CONFIG_SHIPPING 1
#else
	#define CONFIG_SHIPPING 0
#endif // TE_SHIPPING

#if !CONFIG_SHIPPING
	#define CONFIG_DEBUG 1
#else
	#define CONFIG_DEBUG  0
#endif // TE_DEBUG

#if CONFIG_DEBUG
	#define CONFIG_USE_LOGGING 1
	#define TE_DRAW_DEBUG 1
#else
	#define CONFIG_USE_LOGGING 0
	#define TE_DRAW_DEBUG 0
#endif

#ifdef IS_EDITOR
	#define CONFIG_USE_EDITOR 1
#else
	#define CONFIG_USE_EDITOR 0
#endif // IS_EDITOR

#if CONFIG_USE_EDITOR
	#define WITH_EDITOR 1
#else
	#define WITH_EDITOR 0
#endif // CONFIG_USE_EDITOR

// For headless server builds
#define CONFIG_HEADLESS 0

// For executable game builds
#define CONFIG_RUNTIME ( !CONFIG_USE_EDITOR )

//=================================================================================================
// Asset System Configuration
#define USE_ASSET_IMPORTERS ( WITH_EDITOR )
#define USE_ASSET_BUNDLE 0 //( !WITH_EDITOR )
//=================================================================================================