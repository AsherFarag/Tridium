#pragma once
#include "../../RHI.h"
#include "D3D12.h"
#include "D3D12Util.h"
#include "D3D12ToRHI.h"
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Graphics/RHI/RHIVertexLayout.h>
#include <Tridium/Graphics/RHI/RHISampler.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHIShader.h>
#include <Tridium/Graphics/RHI/RHIShaderBindingLayout.h>
#include <Tridium/Graphics/RHI/RHISwapChain.h>
#include <Tridium/Graphics/RHI/RHIDescriptorAllocator.h>
#include <Tridium/Graphics/RHI/RHIFence.h>

DECLARE_LOG_CATEGORY( DirectX );

#if RHI_USE_DEBUG_NAMES
	#define D3D12_SET_DEBUG_NAME( _Object, _Name ) \
	do { \
		if ( RHIQuery::IsDebug() && !_Name.empty() ) \
		{ \
			::Tridium::WString wName( _Name.begin(), _Name.end() ); \
			_Object->SetName( wName.c_str() ); \
			::Tridium::D3D12Context::Get()->StringStorage.EmplaceBack( std::move( wName ) ); \
		} \
	} while ( false )
#else
	#define D3D12_SET_DEBUG_NAME( _Object, _Name )
#endif // RHI_USE_DEBUG_NAMES