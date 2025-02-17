#pragma once
#include <Tridium/Graphics/RHI/RHICommon.h>

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#include <d3d12.h>
#include <dxgi1_6.h>
	
#if RHI_DEBUG_ENABLED
	#include <d3d12sdklayers.h>
	#include <dxgidebug.h>
#endif