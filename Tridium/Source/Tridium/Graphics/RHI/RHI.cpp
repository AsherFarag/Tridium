#include "tripch.h"
#include "RHI.h"

// Backends
#if RHI_ENABLE_BACKEND_DIRECTX12
	#include "Backend/DirectX12/RHI_D3D12Impl.h"
#endif
#if RHI_ENABLE_BACKEND_OPENGL
	#include "Backend/OpenGL/RHI_OpenGLImpl.h"
#endif

#if RHI_ENABLE_BACKEND_VULKAN
	#include "Backend/Vulkan/RHI_VulkanImpl.h"
#endif // RHI_ENABLE_BACKEND_VULKAN

namespace Tridium {

	IDynamicRHI* s_DynamicRHI = nullptr;
	static RHIStats s_LastFrameStats{};

#if RHI_DEBUG_ENABLED
	bool RHI::IsDebug()
	{
		return s_DynamicRHI ? s_DynamicRHI->Config().UseDebug : false;
	}
#endif // RHI_DEBUG_ENABLED

	//////////////////////////////////////////////////////////////////////////
	// RHI CORE FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	bool RHI::Initialise( const RHIConfig& a_Config )
	{
		if ( !ASSERT( s_DynamicRHI == nullptr, "RHI has already been previously initialised!" ) )
		{
			return false;
		}

		switch ( a_Config.RHIType )
		{
		#if RHI_ENABLE_BACKEND_OPENGL
			case ERHInterfaceType::OpenGL:
			{
				s_DynamicRHI = new OpenGL::DynamicRHI_OpenGLImpl();
				break;
			}
		#endif
		#if RHI_ENABLE_BACKEND_DIRECTX12
			case ERHInterfaceType::DirectX12:
			{
				s_DynamicRHI = new D3D12::DynamicRHI_D3D12Impl();
				break;
			}
		#endif
		#if RHI_ENABLE_BACKEND_VULKAN
			case ERHInterfaceType::Vulkan:
			{
				s_DynamicRHI = new Vulkan::DynamicRHI_VulkanImpl();
				break;
			}
		#endif // RHI_ENABLE_BACKEND_VULKAN
			default:
			{
				// Unsupported RHI type
				LOG( LogCategory::RHI, Error, "Unsupported RHI type: {0}", ToString( a_Config.RHIType ) );
				return false;
			}
		}

		ScopeGuard shutdownGuard( [] { delete s_DynamicRHI; s_DynamicRHI = nullptr; } );

		if ( s_DynamicRHI->Init( a_Config ) == false )
			return false; // Failed to initialise the rendering API

		shutdownGuard.Dismiss();
		return true;
	}

	bool RHI::Shutdown()
	{
		RHI_DEV_CHECK( s_DynamicRHI, "RHI is not initialised!" );

		bool success = s_DynamicRHI->Shutdown();

		delete s_DynamicRHI;
		s_DynamicRHI = nullptr;
		return success;
	}

	void RHI::BeginFrame()
	{
		RHI_DEV_CHECK( s_DynamicRHI, "RHI is not initialised!" );
		s_DynamicRHI->BeginFrame();
	}

	void RHI::EndFrame()
	{
		RHI_DEV_CHECK( s_DynamicRHI, "RHI is not initialised!" );
		s_DynamicRHI->EndFrame();
		if ( s_DynamicRHI->QueryRHIStats( s_LastFrameStats ) == false )
		{
			LOG( LogCategory::RHI, Warn, "Failed to query RHI stats!" );
			s_LastFrameStats.Reset();
		}
	}

	bool RHI::Present()
	{
		IRHISwapChain* swapChain = s_DynamicRHI->GetSwapChain();
		if ( swapChain == nullptr )
			return false;

		return swapChain->Present();
	}

	RHIFenceValue RHI::ExecuteCommandLists( Span<IRHICommandList* const> a_CommandLists, ERHICommandQueueType a_QueueType )
	{
		RHI_DEV_CHECK( s_DynamicRHI, "RHI is not initialised!" );
		return s_DynamicRHI->ExecuteCommandLists( a_CommandLists, a_QueueType );
	}

	bool RHI::WaitForIdle()
	{
		RHI_DEV_CHECK( s_DynamicRHI, "RHI is not initialised!" );
		return s_DynamicRHI->WaitForIdle();
	}

	void RHI::WaitForFence( ERHICommandQueueType a_QueueType, RHIFenceValue a_FenceValue )
	{
		RHI_DEV_CHECK( s_DynamicRHI, "RHI is not initialised!" );
		s_DynamicRHI->WaitForFence( a_QueueType, a_FenceValue );
	}

	void RHI::CollectGarbage()
	{
		RHI_DEV_CHECK( s_DynamicRHI, "RHI is not initialised!" );
		s_DynamicRHI->CollectGarbage();
	}

	StringView RHI::GetRHIName()
	{
		if ( s_DynamicRHI == nullptr )
		{
			return "Null";
		}

		return RHI::GetRHIName( s_DynamicRHI->GetRHIType() );
	}

	const RHIStats& RHI::GetRHIStats()
	{
		return s_LastFrameStats;
	}

	//////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHITextureRef RHI::CreateTexture( const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateTexture( a_Desc, a_SubResourcesData );
	}

	RHIBufferRef RHI::CreateBuffer( const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateBuffer( a_Desc, a_Data );
	}

	RHICommandListRef RHI::CreateCommandList( const RHICommandListDesc& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateCommandList( a_Desc );
	}

	RHISwapChainRef RHI::CreateSwapChain( const RHISwapChainDesc& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateSwapChain( a_Desc );
	}

	RHIShaderModuleRef RHI::CreateShaderModule( const RHIShaderModuleDesc& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateShaderModule( a_Desc );
	}

	RHIBindingLayoutRef RHI::CreateBindingLayout( const RHIBindingLayoutDesc& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateBindingLayout( a_Desc );
	}

	RHIBindingSetRef RHI::CreateBindingSet( const RHIBindingSetDesc& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateBindingSet( a_Desc );
	}

	RHIGraphicsPipelineStateRef RHI::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDesc& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateGraphicsPipelineState( a_Desc );
	}

	//////////////////////////////////////////////////////////////////////////

} // namespace Tridium
