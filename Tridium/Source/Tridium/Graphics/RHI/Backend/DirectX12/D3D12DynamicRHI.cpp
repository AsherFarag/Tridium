#include "tripch.h"
#include "D3D12DynamicRHI.h"

// For getting the native window handle
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>


// Resources
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIVertexLayout.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>

// Backend resources
#include "D3D12Sampler.h"
#include "D3D12Texture.h"
#include "D3D12Buffer.h"
#include "D3D12Shader.h"
#include "D3D12Mesh.h"
#include "D3D12PipelineState.h"
#include "D3D12ShaderBindingLayout.h"
#include "D3D12CommandList.h"
#include "D3D12SwapChain.h"
#include "D3D12Fence.h"

namespace Tridium {

	//////////////////////////////////////////////////////////////////////////
	// CORE RHI FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

    bool D3D12RHI::Init( const RHIConfig& a_Config )
    {
	#if RHI_DEBUG_ENABLED
		if ( a_Config.UseDebug )
		{
			// Init D3D12 Debug layer
			if ( FAILED( D3D12GetDebugInterface( IID_PPV_ARGS( &m_D3D12Debug ) ) ) )
			{
				LOG( LogCategory::DirectX, Error, "Failed to create D3D12 debug interface!" );
				return false;
			}

			m_D3D12Debug->EnableDebugLayer();

			// Init DXGI Debug
			if ( FAILED( DXGIGetDebugInterface1( 0, IID_PPV_ARGS( &m_DXGIDebug ) ) ) )
			{
				LOG( LogCategory::DirectX, Error, "Failed to create DXGI debug interface!" );
				return false;
			}

			m_DXGIDebug->EnableLeakTrackingForThread();
		}
	#endif

		// Create the DXGIFactory
        if ( FAILED( CreateDXGIFactory2( 0, IID_PPV_ARGS( &m_DXGIFactory ) ) ) )
        {
			LOG( LogCategory::DirectX, Error, "Failed to create DXGIFactory!" );
            return false;
        }

		// Create the device
        if ( FAILED( D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &m_Device ) ) ) )
        {
			LOG( LogCategory::DirectX, Error, "Failed to create D3D12 device!" );
            return false;
        }

		// Get the max supported D3D12 device version
#define QUERY_D3D12_DEVICE_VERSION( _Version ) \
		if ( ID3D12Device##_Version* deviceVersion = nullptr; SUCCEEDED( m_Device->QueryInterface<ID3D12Device##_Version>( &deviceVersion ) ) ) \
		{ \
			m_MaxD3D12DeviceVersion = _Version; \
		}

		QUERY_D3D12_DEVICE_VERSION( 1 );
		QUERY_D3D12_DEVICE_VERSION( 2 );
		QUERY_D3D12_DEVICE_VERSION( 3 );
		QUERY_D3D12_DEVICE_VERSION( 4 );
		QUERY_D3D12_DEVICE_VERSION( 5 );

#undef QUERY_D3D12_DEVICE_VERSION



		// Retrieve the adapter
		if ( FAILED( m_DXGIFactory->EnumAdapters( 0, &m_DXGIAdapter ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to enumerate adapters!" );
			return false;
		}

		// Set up Command Contexts
		for ( size_t i = 0; i < m_CmdContexts.Size(); ++i )
		{
			// Set up the command context and set the command queue type
			D3D12CommandContext& cmdCtx = m_CmdContexts[i];

			ERHICommandQueueType cmdQueueType = static_cast<ERHICommandQueueType>( i );
			D3D12_COMMAND_LIST_TYPE d3d12CmdListType;
			switch ( cmdQueueType )
			{
				case ERHICommandQueueType::Graphics:  d3d12CmdListType = D3D12_COMMAND_LIST_TYPE_DIRECT;  break;
				case ERHICommandQueueType::Compute: d3d12CmdListType = D3D12_COMMAND_LIST_TYPE_COMPUTE; break;
				case ERHICommandQueueType::Copy:    d3d12CmdListType = D3D12_COMMAND_LIST_TYPE_COPY;    break;
				default: ASSERT( false, "Invalid command queue type!" ); break;
			}

			// Create the command queue
			{
				D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
				cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
				cmdQueueDesc.NodeMask = 0;
				cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				cmdQueueDesc.Type = d3d12CmdListType;

				if ( FAILED( m_Device->CreateCommandQueue( &cmdQueueDesc, IID_PPV_ARGS( &cmdCtx.CmdQueue ) ) ) )
				{
					return false;
				}
			}

			// Create the command allocator
			if ( FAILED( m_Device->CreateCommandAllocator( d3d12CmdListType, IID_PPV_ARGS( &cmdCtx.CmdAllocator ) ) ) )
			{
				return false;
			}

			// Create the command list
			if ( FAILED( GetD3D12Device4()->CreateCommandList1(0, d3d12CmdListType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&cmdCtx.CmdList))) )
			{
				return false;
			}

			// Create the fence
			cmdCtx.FenceValue = 0;
			cmdCtx.FenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
			if ( cmdCtx.FenceEvent == nullptr )
			{
				return false;
			}
			if ( FAILED( m_Device->CreateFence( cmdCtx.FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &cmdCtx.Fence ) ) ) )
			{
				return false;
			}

			TODO( "Temp" );
			cmdCtx.CmdQueue->SetName( L"CmdCtx-RHICommandQueue" );
			cmdCtx.CmdAllocator->SetName( L"CmdCtx-RHICommandAllocator" );
			cmdCtx.Fence->SetName( L"CmdCtx-RHICommandFence" );
		}

		// Create the Memory Allocator
		using enum D3D12MA::ALLOCATOR_FLAGS;
		int allocatorFlags = D3D12MA::ALLOCATOR_FLAG_NONE;
		allocatorFlags |= ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;
		allocatorFlags |= ALLOCATOR_FLAG_MSAA_TEXTURES_ALWAYS_COMMITTED;
		allocatorFlags |= !RHIQuery::SupportsMultithreading() ? ALLOCATOR_FLAG_SINGLETHREADED : ALLOCATOR_FLAG_NONE;
		D3D12MA::ALLOCATOR_DESC  allocatorDesc = {
			.Flags = D3D12MA::ALLOCATOR_FLAGS( allocatorFlags ),
			.pDevice = m_Device.Get(),
			.PreferredBlockSize = 0,
			.pAllocationCallbacks = nullptr,
			.pAdapter = m_DXGIAdapter.Get()
		};
		allocatorDesc.pDevice = m_Device.Get();
		allocatorDesc.pAdapter = m_DXGIAdapter.Get();
		allocatorDesc.PreferredBlockSize = 0;
		if ( FAILED( D3D12MA::CreateAllocator( &allocatorDesc, &m_Allocator ) ) )
		{
			return false;
		}

		// Init the descriptor heap manager
		TODO( "Figure out some better values/where to get them from" );
		const uint32_t numResourceDescriptors = 2048;
		const uint32_t numSamplerDescriptors = 512;
		m_DescriptorHeapManager.Init( m_Device.Get(), numResourceDescriptors, numSamplerDescriptors );

		// Create the upload buffer
		if ( !m_UploadBuffer.Commit( 1024 * 1024 * 64, *m_Allocator ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create upload buffer" );
			return false;
		}


        return true;
    }

	bool D3D12RHI::Shutdown()
	{
		m_UploadBuffer.Release();

		m_Device.Release();

		m_DXGIFactory.Release();

    #if RHI_DEBUG_ENABLED
		DumpDebug();
		m_DXGIDebug.Release();
		m_D3D12Debug.Release();
    #endif

		return true;
	}

    bool D3D12RHI::ExecuteCommandList( RHICommandListRef a_CommandList )
    {
		D3D12CommandList* cmdList = a_CommandList->As<D3D12CommandList>();
		if ( FAILED( cmdList->GraphicsCommandList()->Close() ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to close command list" );
			return false;
		}

		auto& cmdCtx = GetCommandContext( a_CommandList->Descriptor().QueueType );
		cmdCtx.CmdQueue->ExecuteCommandLists( 1, &cmdList->CommandList );
		a_CommandList->SetFenceValue( cmdCtx.Signal() );

		return true;
    }

    //////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHIFenceRef D3D12RHI::CreateFence( const RHIFenceDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIFence_D3D12Impl>( a_Desc );
	}

	RHISamplerRef D3D12RHI::CreateSampler( const RHISamplerDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHISampler_D3D12Impl>( a_Desc );
	}

	RHITextureRef D3D12RHI::CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
	{
		return RHI::CreateNativeResource<RHITexture_D3D12Impl>( a_Desc, a_SubResourcesData );
	}

	RHIBufferRef D3D12RHI::CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data )
	{
		return RHI::CreateNativeResource<D3D12Buffer>( a_Desc, a_Data );
	}

	RHIGraphicsPipelineStateRef D3D12RHI::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIGraphicsPipelineState_D3D12Impl>( a_Desc );
	}

	RHICommandListRef D3D12RHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<D3D12CommandList>( a_Desc );
	}

	RHIShaderModuleRef D3D12RHI::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIShaderModule_D3D12Impl>( a_Desc );
	}

	RHIBindingLayoutRef D3D12RHI::CreateBindingLayout( const RHIBindingLayoutDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIBindingLayout_D3D12Impl>( a_Desc );
	}

	RHIBindingSetRef D3D12RHI::CreateBindingSet( const RHIBindingSetDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIBindingSet_D3D12Impl>( a_Desc );
	}

	RHISwapChainRef D3D12RHI::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHISwapChain_D3D12Impl>( a_Desc );
	}

	//////////////////////////////////////////////////////////////////////////
    // DEBUG
	//////////////////////////////////////////////////////////////////////////

#if RHI_DEBUG_ENABLED
    void D3D12RHI::DumpDebug()
    {
        if ( m_DXGIDebug )
        {
            OutputDebugStringW( L"DirectX12 Debug Dump:\n" );
            m_DXGIDebug->ReportLiveObjects(
                DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL )
            );
			OutputDebugStringW( L"End of DirectX12 Debug Dump\n" );
        }
    }
#endif // RHI_DEBUG_ENABLED

} // namespace Tridium
