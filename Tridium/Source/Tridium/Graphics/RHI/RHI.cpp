#include "tripch.h"
#include "RHI.h"

// Backends
#if RHI_ENABLE_BACKEND_DIRECTX12
	#include "Backend/DirectX12/D3D12DynamicRHI.h"
#endif
#if RHI_ENABLE_BACKEND_OPENGL
	#include "Backend/OpenGL/OpenGLDynamicRHI.h"
#endif

namespace Tridium {

	IDynamicRHI* s_DynamicRHI = nullptr;

	//////////////////////////////////////////////////////////////////////////
	// RHI CORE FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	bool RHI::Initialise( const RHIConfig& a_Config )
	{
		if ( !ASSERT_LOG( !s_RHIGlobals.IsRHIInitialised, "RHI has already been previously initialised!" ) )
		{
			return false;
		}

		switch ( a_Config.RHIType )
		{
		#if RHI_ENABLE_BACKEND_OPENGL
			case ERHInterfaceType::OpenGL:
			{
				s_DynamicRHI = new OpenGLDynamicRHI();
				break;
			}
		#endif
		#if RHI_ENABLE_BACKEND_DIRECTX12
			case ERHInterfaceType::DirectX12:
			{
				s_DynamicRHI = new D3D12RHI();
				break;
			}
		#endif
		}

		if ( s_DynamicRHI == nullptr )
		{
			return ASSERT( false );
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
			desc.Format = ERHITextureFormat::RGBA8;
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

		return true;
	}

	bool RHI::Shutdown()
	{

		RHI::FenceSignal( RHI::CreateFence() );

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
			s_RHIGlobals.FrameIndex++;
			s_RHIGlobals.FrameIndex %= RHIConstants::MaxFrameBuffers;
		}

		return success;
	}

	bool RHI::ExecuteCommandList( const RHICommandListRef& a_CommandList )
	{
		if ( !ASSERT_LOG( s_RHIGlobals.IsRHIInitialised, "RHI has not been initialised!" ) )
		{
			return false;
		}

		return s_DynamicRHI->ExecuteCommandList( a_CommandList );
	}

	uint32_t RHI::FrameIndex()
	{
		return s_RHIGlobals.FrameIndex;
	}

	const char* RHI::GetRHIName()
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
	// FENCE FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	RHIFence RHI::CreateFence()
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateFence();
	}

	ERHIFenceState RHI::GetFenceState( RHIFence a_Fence )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->GetFenceState( a_Fence );
	}

	void RHI::FenceSignal( RHIFence a_Fence )
	{
		CHECK( s_DynamicRHI );
		s_DynamicRHI->FenceSignal( a_Fence );
	}

	//////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHISamplerRef RHI::CreateSampler( const RHISamplerDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateSampler( a_Desc );
	}

	RHITextureRef RHI::CreateTexture( const RHITextureDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateTexture( a_Desc );
	}

	RHIIndexBufferRef RHI::CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateIndexBuffer( a_Desc );
	}

	RHIVertexBufferRef RHI::CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateVertexBuffer( a_Desc );
	}

	RHIGraphicsPipelineStateRef RHI::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateGraphicsPipelineState( a_Desc );
	}

	RHICommandListRef RHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateCommandList( a_Desc );
	}

	RHIShaderModuleRef RHI::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateShaderModule( a_Desc );
	}

	RHIShaderBindingLayoutRef RHI::CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateShaderBindingLayout( a_Desc );
	}

	RHISwapChainRef RHI::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreateSwapChain( a_Desc );
	}

	RHITextureRef RHI::CreateTexture2D( uint32_t a_Width, uint32_t a_Height, Span<const uint8_t> a_Data, ERHITextureFormat a_Format, const char* a_Name, ERHIUsageHint a_Usage )
	{
		const size_t ExpectedSize = a_Width * a_Height * GetTextureFormatSize( a_Format );
		if ( !ASSERT_LOG( a_Data.size() == ExpectedSize, "Data size does not match the expected size for the given texture format!" ) )
		{
			return nullptr;
		}

		RHITextureDescriptor desc;
		desc.Name = a_Name;
		desc.Width = a_Width;
		desc.Height = a_Height;
		desc.InitialData = a_Data;
		desc.Format = a_Format;
		desc.UsageHint = a_Usage;
		return CreateTexture( desc );
	}

	RHIIndexBufferRef RHI::CreateIndexBuffer( Span<const Byte> a_InitialData, ERHIDataType a_DataType, const char* a_Name, ERHIUsageHint a_UsageHint )
	{
		RHIIndexBufferDescriptor desc;
		desc.Name = a_Name;
		desc.InitialData = a_InitialData;
		desc.UsageHint = a_UsageHint;
		desc.DataType = a_DataType;
		return CreateIndexBuffer( desc );
	}

	RHIVertexBufferRef RHI::CreateVertexBuffer( Span<const Byte> a_InitialData, RHIVertexLayout a_Layout, const char* a_Name, ERHIUsageHint a_UsageHint )
	{
		RHIVertexBufferDescriptor desc;
		desc.Name = a_Name;
		desc.InitialData = a_InitialData;
		desc.Layout = a_Layout;
		desc.UsageHint = a_UsageHint;
		return CreateVertexBuffer( desc );
	}

	//////////////////////////////////////////////////////////////////////////

} // namespace Tridium
