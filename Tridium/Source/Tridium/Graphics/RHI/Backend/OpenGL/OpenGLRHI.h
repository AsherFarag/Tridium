#pragma once
#include <Tridium/Graphics/RHI/RHICommon.h>
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/String.h>

// Resources
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIMesh.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>


namespace Tridium {

	class OpenGLRHI final : public IDynamicRHI
	{
	public:
		//==============================================
		// Core RHI functions
		// Initialise the RHI with the given configuration.
		virtual bool Init( const RHIConfig& a_Config ) override;
		// Shutdown the RHI.
		virtual bool Shutdown() override;
		// Execute the given command list.
		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) override;
		// Returns the type of the Dynamically bound RHI.
		virtual ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::OpenGL; }
		// Returns the static RHI type.
		static constexpr ERHInterfaceType GetStaticRHIType() { return ERHInterfaceType::OpenGL; }
		//==============================================

		//====================================================
		// Creates a fence that can be used to synchronize the CPU and GPU.
		virtual RHIFence CreateFence() const override;
		// Queries the state of a fence.
		virtual ERHIFenceState GetFenceState( RHIFence a_Fence ) const override;
		// Blocks the calling CPU thread until the fence is signaled by the GPU.
		virtual void FenceSignal( RHIFence a_Fence ) override;
		//=====================================================

		//=====================================================
		// Resource creation
		virtual RHISamplerRef CreateSampler( const RHISamplerDescriptor& a_Desc ) override;
		virtual RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc ) override;
		virtual RHIIndexBufferRef CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc ) override;
		virtual RHIVertexBufferRef CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc ) override;
		virtual RHIPipelineStateRef CreatePipelineState( const RHIPipelineStateDescriptor& a_Desc ) override;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc ) override;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc ) override;
		virtual RHIShaderBindingLayoutRef CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc ) override;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDescriptor& a_Desc ) override;
		//=====================================================

		#if RHI_DEBUG_ENABLED

		// Dump debug information about the RHI into the console.
		virtual void DumpDebug() override;

		#endif // RHI_DEBUG_ENABLED
	};

	namespace RHI {
		static OpenGLRHI* GetOpenGLRHI()
		{
		#if RHI_DEBUG_ENABLED
			if ( s_DynamicRHI->GetRHIType() != ERHInterfaceType::OpenGL )
			{
				ASSERT_LOG( false, "The current RHI is not OpenGL!" );
				return nullptr;
			}
		#endif
			return static_cast<OpenGLRHI*>( s_DynamicRHI );
		}
	} // namespace RHI

} // namespace Tridium