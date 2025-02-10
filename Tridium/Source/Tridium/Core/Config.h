#pragma once

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