#pragma once

#ifdef TE_DEBUG
	#define CONFIG_DEBUG 1
#else
	#define CONFIG_DEBUG  0
#endif // TE_DEBUG

#ifdef TE_SHIPPING
	#define CONFIG_SHIPPING 1
#else
	#define CONFIG_SHIPPING 0
#endif // TE_SHIPPING

#if CONFIG_DEBUG
	#define TE_DRAW_DEBUG 1
#else
	#define TE_DRAW_DEBUG 0
#endif