#pragma once
#include "RHICommon.h"
#include "DynamicRHI.h"
#include "RHITexture.h"
#include "RHIVertexLayout.h"
#include "RHIPipelineState.h"
#include "ShaderLibrary.h"
#include "RHIBuffer.h"
#include "RHICommandList.h"
#include "RHIShaderBindings.h"
#include "RHISwapChain.h"
#include "RHIFence.h"

namespace Tridium {

	namespace RHI {

		//===========================
		// Core RHI functions
		
		// Initialise the RHI with the given configuration.
		bool Initialise( const RHIConfig& a_Config );
		// Shutdown the RHI.
		bool Shutdown();
		// Present the current frame to the screen using the swap chain.
		bool Present();
		// Execute the given command list on the GPU.
		bool ExecuteCommandList( const RHICommandListRef& a_CommandList );

		// Wait for the frame fence to complete.
		void FrameFenceWait();
		//===========================

		//===========================
		// RHI Query and Functions
		uint32_t FrameIndex();
		RHIFeatureInfo GetFeatureInfo( ERHIFeature a_Feature );
		ERHIFeatureSupport GetFeatureSupport( ERHIFeature a_Feature );
		bool IsFeatureSupported( ERHIFeature a_Feature );
		//===========================

		//===========================
		// Resource creation
		[[nodiscard]] RHIFenceRef CreateFence( const RHIFenceDesc& a_Desc );
		[[nodiscard]] RHISamplerRef CreateSampler( const RHISamplerDesc& a_Desc );
		[[nodiscard]] RHITextureRef CreateTexture( const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData = {} );
		[[nodiscard]] RHIBufferRef CreateBuffer( const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data = {} );
		[[nodiscard]] RHICommandListRef CreateCommandList( const RHICommandListDesc& a_Desc );
		[[nodiscard]] RHISwapChainRef CreateSwapChain( const RHISwapChainDesc& a_Desc );
		[[nodiscard]] RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDesc& a_Desc );
		[[nodiscard]] RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDesc& a_Desc );
		[[nodiscard]] RHIBindingSetRef CreateBindingSet( const RHIBindingSetDesc& a_Desc );
		[[nodiscard]] RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDesc& a_Desc );

		// Constructs a specific IRHIObject Implementation and registers it with the RHI.
		// Used for creating resources that are not created by the RHI via RHI::CreateResource calls.
		// Useful for also creating RHIResources from native API resources.
		template<Concepts::IsRHIResourceImplementation _NativeResource, typename... _Args>
		[[nodiscard]] static typename _NativeResource::RefType CreateNativeObject( _Args&&... a_Args )
		{
			return IRHIObject::CreateHandle( new _NativeResource( std::forward<_Args>( a_Args )... ) );
		}

		// Creates a RHI Reference to the given resource and registers it with the RHI.
		// Used for creating resources that are not created by the RHI via RHI::CreateResource calls.
		// Useful for also creating RHIResources from native API resources.
		template<Concepts::IsRHIResourceImplementation T>
		[[nodiscard]] static typename T::RefType CreateNativeObject( T* a_Resource )
		{
			return IRHIObject::CreateHandle( a_Resource );
		}

		// Wrappers

		[[nodiscard]] inline RHITextureRef CreateTexture( const RHITextureDesc& a_Desc, RHITextureSubresourceData a_SubResourcesData )
		{
			return CreateTexture( a_Desc, Span<RHITextureSubresourceData>{ &a_SubResourcesData, 1 } );
		}
		//===========================
	}

}
