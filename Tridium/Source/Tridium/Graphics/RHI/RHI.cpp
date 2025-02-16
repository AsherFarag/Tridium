#include "tripch.h"
#include "RHI.h"

// Backends
#if RHI_ENABLE_BACKEND_DIRECTX12
	#include "Backend/DirectX12/D3D12RHI.h"
#endif
#if RHI_ENABLE_BACKEND_OPENGL
	#include "Backend/OpenGL/OpenGLRHI.h"
#endif

namespace Tridium {

	DynamicRHI* s_DynamicRHI = nullptr;

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
				s_DynamicRHI = new OpenGLRHI();
				break;
			}
		#endif
		#if RHI_ENABLE_BACKEND_DIRECTX12
			case ERHInterfaceType::DirectX12:
			{
				s_DynamicRHI = new DirectX12RHI();
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

		return true;
	}

	bool RHI::Shutdown()
	{
		if ( s_DynamicRHI == nullptr )
		{
			return false;
		}

		// Shutdown the rendering API
		bool success = s_DynamicRHI->Shutdown();

		delete s_DynamicRHI;
		s_DynamicRHI = nullptr;
		return success;
	}

	bool RHI::Present()
	{
		if ( !ASSERT_LOG( s_RHIGlobals.IsRHIInitialised, "RHI has not been initialised!" ) )
		{
			return false;
		}

		return s_DynamicRHI->Present();
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

	RHIPipelineStateRef RHI::CreatePipelineState( const RHIPipelineStateDescriptor& a_Desc )
	{
		CHECK( s_DynamicRHI );
		return s_DynamicRHI->CreatePipelineState( a_Desc );
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

	RHITextureRef RHI::CreateTexture( uint32_t a_Width, uint32_t a_Height, Span<const uint8_t> a_Data, ERHITextureFormat a_Format, const char* a_Name, ERHIUsageHint a_Usage, SharedPtr<RHIResourceAllocator> a_Allocator )
	{
		const size_t ExpectedSize = a_Width * a_Height * GetTextureFormatSize( a_Format );
		if ( !ASSERT_LOG( a_Data.size() == ExpectedSize, "Data size does not match the expected size for the given texture format!" ) )
		{
			return nullptr;
		}

		RHITextureDescriptor desc;
		desc.Name = a_Name;
		desc.Dimensions[0] = a_Width;
		desc.Dimensions[1] = a_Height;
		desc.InitialData = a_Data;
		desc.Format = a_Format;
		desc.UsageHint = a_Usage;
		desc.Allocator = a_Allocator;

		return CreateTexture( desc );
	}

	RHIIndexBufferRef RHI::CreateIndexBuffer( Span<const Byte> a_InitialData, ERHIDataType a_DataType, const char* a_Name, ERHIUsageHint a_UsageHint, SharedPtr<RHIResourceAllocator> a_Allocator )
	{
		RHIIndexBufferDescriptor desc;
		desc.Name = a_Name;
		desc.InitialData = a_InitialData;
		desc.UsageHint = a_UsageHint;
		desc.DataType = a_DataType;
		desc.Allocator = a_Allocator;
		return CreateIndexBuffer( desc );
	}

	RHIVertexBufferRef RHI::CreateVertexBuffer( Span<const Byte> a_InitialData, RHIVertexLayout a_Layout, const char* a_Name, ERHIUsageHint a_UsageHint, SharedPtr<RHIResourceAllocator> a_Allocator )
	{
		RHIVertexBufferDescriptor desc;
		desc.Name = a_Name;
		desc.InitialData = a_InitialData;
		desc.Layout = a_Layout;
		desc.UsageHint = a_UsageHint;
		desc.Allocator = a_Allocator;
		return CreateVertexBuffer( desc );
	}

	//////////////////////////////////////////////////////////////////////////

} // namespace Tridium
