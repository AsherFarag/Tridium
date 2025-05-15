#include "tripch.h"
#include "RHI.h"

// Backends
#if RHI_ENABLE_BACKEND_DIRECTX12
	#include "Backend/DirectX12/RHI_D3D12Impl.h"
#endif
#if RHI_ENABLE_BACKEND_OPENGL
	#include "Backend/OpenGL/RHI_OpenGLImpl.h"
#endif

namespace Tridium {

	IDynamicRHI* s_DynamicRHI = nullptr;

	//////////////////////////////////////////////////////////////////////////
	// RHI CORE FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	bool RHI::Initialise( const RHIConfig& a_Config )
	{
		if ( !ASSERT( !s_RHIGlobals.IsRHIInitialised, "RHI has already been previously initialised!" ) )
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
			default:
			{
				// Unsupported RHI type
				LOG( LogCategory::RHI, Error, "Unsupported RHI type: ", static_cast<uint32_t>(a_Config.RHIType) );
				return false;
			}
		}

		if ( !ASSERT( s_DynamicRHI ) )
		{
			return false;
		}

		if ( s_DynamicRHI->Init( a_Config ) == false )
		{
			// Failed to initialise the rendering API
			delete s_DynamicRHI;
			s_DynamicRHI = nullptr;
			return false;
		}

		s_RHIGlobals.IsRHIInitialised = true;
		s_RHIGlobals.Config = a_Config;
		TODO( "Set up proper Multithreading query" );
		s_RHIGlobals.SupportsMultithreading = a_Config.SingleThreaded == false;
		
		if ( a_Config.CreateSwapChain )
		{
			RHISwapChainDescriptor desc;
			desc.Width = 1280;
			desc.Height = 720;
			desc.BufferCount = 2;
			desc.Format = ERHIFormat::RGBA8_UNORM;
			desc.Flags = ERHISwapChainFlags::UseVSync;
			s_RHIGlobals.SwapChain = RHI::CreateSwapChain( desc );
			if ( s_RHIGlobals.SwapChain == nullptr )
			{
				// Failed to create the swap chain
				delete s_DynamicRHI;
				s_DynamicRHI = nullptr;
				return false;
			}
		}

		// Create the frame fence
		s_RHIGlobals.Fence = RHI::CreateFence();
		// Set the initial frame index
		s_RHIGlobals.FrameIndex = 0;
		// And initialise the fence values
		s_RHIGlobals.FrameFenceValue = 0;
		//s_RHIGlobals.FrameFenceValues.Fill( 0, a_Config.MaxFramesInFlight );
		//for ( uint64_t& fenceValue : s_RHIGlobals.FrameFenceValues )
		//{
		//	fenceValue = 0;
		//}

		return true;
	}

	bool RHI::Shutdown()
	{
		TODO( "This" );
		RHI::FrameFenceWait();

		if ( s_DynamicRHI == nullptr )
		{
			return false;
		}

		s_RHIGlobals = RHIGlobals();

		// Shutdown the rendering API
		bool success = s_DynamicRHI->Shutdown();

		delete s_DynamicRHI;
		s_DynamicRHI = nullptr;
		return success;
	}

	bool RHI::Present()
	{
		RHISwapChainRef swapChain = RHI::GetSwapChain();
		if ( swapChain == nullptr )
		{
			return false;
		}

		bool success = swapChain->Present();
		if ( success )
		{
			++s_RHIGlobals.FrameIndex;
			s_RHIGlobals.FrameIndex %= RHIConstants::MaxFrameBuffers;
		}

		return success;
	}

	bool RHI::ExecuteCommandList( const RHICommandListRef& a_CommandList )
	{
		if ( !ASSERT( s_RHIGlobals.IsRHIInitialised, "RHI has not been initialised!" ) )
		{
			return false;
		}

		bool success = s_DynamicRHI->ExecuteCommandList( a_CommandList );
		if ( !success )
		{
			LOG( LogCategory::RHI, Error, "Failed to execute command list!" );
			return false;
		}

		a_CommandList->SetPendingExecution( true );
	}

	void RHI::FrameFenceWait()
	{
		const RHIFenceRef& fence = RHI::GetGlobalFence();
		if ( fence )
		{
			fence->Wait( ++s_RHIGlobals.FrameFenceValue );
		}
	}

	uint32_t RHI::FrameIndex()
	{
		return s_RHIGlobals.FrameIndex;
	}

	RHIFeatureInfo RHI::GetFeatureInfo( ERHIFeature a_Feature )
	{
		if ( s_DynamicRHI == nullptr )
		{
			return RHIFeatureInfo();
		}
		return s_DynamicRHI->GetDeviceFeatures().GetFeatureInfo( a_Feature );
	}

	ERHIFeatureSupport RHI::GetFeatureSupport( ERHIFeature a_Feature )
	{
		return RHI::GetFeatureInfo( a_Feature ).Support();
	}

	bool RHI::IsFeatureSupported( ERHIFeature a_Feature )
	{
		return RHI::GetFeatureSupport( a_Feature ) == ERHIFeatureSupport::Supported;
	}

	StringView RHI::GetRHIName()
	{
		if ( s_DynamicRHI == nullptr )
		{
			return "Null";
		}

		return RHI::GetRHIName( s_DynamicRHI->GetRHIType() );
	}

	ERHInterfaceType RHI::GetRHIType()
	{
		if ( s_DynamicRHI == nullptr )
		{
			CHECK( false );
			return ERHInterfaceType::Null;
		}

		return s_DynamicRHI->GetRHIType();
	}

	//////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHIFenceRef RHI::CreateFence( const RHIFenceDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateFence( a_Desc );
	}

	RHISamplerRef RHI::CreateSampler( const RHISamplerDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateSampler( a_Desc );
	}

	RHITextureRef RHI::CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateTexture( a_Desc, a_SubResourcesData );
	}

	RHIBufferRef RHI::CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateBuffer( a_Desc, a_Data );
	}

	RHICommandListRef RHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateCommandList( a_Desc );
	}

	RHISwapChainRef RHI::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateSwapChain( a_Desc );
	}

	RHIShaderModuleRef RHI::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateShaderModule( a_Desc );
	}

	RHIBindingLayoutRef RHI::CreateBindingLayout( const RHIBindingLayoutDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateBindingLayout( a_Desc );
	}

	RHIBindingSetRef RHI::CreateBindingSet( const RHIBindingSetDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateBindingSet( a_Desc );
	}

	RHIGraphicsPipelineStateRef RHI::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateGraphicsPipelineState( a_Desc );
	}

	//////////////////////////////////////////////////////////////////////////

} // namespace Tridium
