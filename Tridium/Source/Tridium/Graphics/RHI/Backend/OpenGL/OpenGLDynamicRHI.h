#pragma once
#include <Tridium/Graphics/RHI/RHICommon.h>
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/String.h>

// Resources
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIVertexLayout.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>


namespace Tridium {

	class OpenGLDynamicRHI final : public IDynamicRHI
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

		//=====================================================
		// Resource creation
		virtual RHIFenceRef CreateFence( const RHIFenceDescriptor& a_Desc ) override;
		virtual RHISamplerRef CreateSampler( const RHISamplerDescriptor& a_Desc ) override;
		virtual RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData ) override;
		virtual RHIBufferRef CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data ) override;
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc ) override;
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
		static OpenGLDynamicRHI* GetOpenGLRHI()
		{
		#if RHI_DEBUG_ENABLED
			if ( s_DynamicRHI->GetRHIType() != ERHInterfaceType::OpenGL )
			{
				ASSERT( false, "The current RHI is not OpenGL!" );
				return nullptr;
			}
		#endif
			return static_cast<OpenGLDynamicRHI*>( s_DynamicRHI );
		}
	} // namespace RHI

} // namespace Tridium