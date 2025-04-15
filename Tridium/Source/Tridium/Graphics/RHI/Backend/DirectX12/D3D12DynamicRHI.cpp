#include "tripch.h"
#include "D3D12DynamicRHI.h"

// For getting the native window handle
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>


// Resources
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIMesh.h>
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
				return false;
			}

			m_D3D12Debug->EnableDebugLayer();

			// Init DXGI Debug
			if ( FAILED( DXGIGetDebugInterface1( 0, IID_PPV_ARGS( &m_DXGIDebug ) ) ) )
			{
				return false;
			}

			m_DXGIDebug->EnableLeakTrackingForThread();
		}
	#endif

		// Create the DXGIFactory
        if ( FAILED( CreateDXGIFactory2( 0, IID_PPV_ARGS( &m_DXGIFactory ) ) ) )
        {
            return false;
        }

		// Create the device
        if ( FAILED( D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &m_Device ) ) ) )
        {
            return false;
        }

		// Retrieve the adapter
		if ( FAILED( m_DXGIFactory->EnumAdapters( 0, &m_Adapter ) ) )
		{
			return false;
		}

		// Create the command queue
        D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
        cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        cmdQueueDesc.NodeMask = 0;
        cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        if ( FAILED( m_Device->CreateCommandQueue( &cmdQueueDesc, IID_PPV_ARGS( &m_CommandQueue ) ) ) )
        {
            return false;
        }

		// Create the command allocator and command list
        if ( FAILED( m_Device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &m_CommandAllocator ) ) ) )
        {
            return false;
        }

		// Create the command list
        if ( FAILED( m_Device->CreateCommandList1( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS( &m_CommandList ) ) ) )
        {
            return false;
        }

		// Create the fence TEMP?
		m_Fence = CreateFence( RHIFenceDescriptor() );
		if ( !m_Fence )
		{
			return false;
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
			.pAdapter = m_Adapter.Get()
		};
		allocatorDesc.pDevice = m_Device.Get();
		allocatorDesc.pAdapter = m_Adapter.Get();
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

		// Init Copy Objects
		{
			// Create the copy command queue
			D3D12_COMMAND_QUEUE_DESC copyQueueDesc{};
			copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			copyQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			copyQueueDesc.NodeMask = 0;
			copyQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			if ( FAILED( m_Device->CreateCommandQueue( &copyQueueDesc, IID_PPV_ARGS( &m_CopyQueue ) ) ) )
			{
				return false;
			}
			D3D12_SET_DEBUG_NAME( m_CopyQueue.Get(), StringView{ "RHICopyCmdQueue" } );

			// Create the copy command allocator
			if ( FAILED( m_Device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS( &m_CopyAllocator ) ) ) )
			{
				return false;
			}
			D3D12_SET_DEBUG_NAME( m_CopyAllocator.Get(), StringView{ "RHICopyCmdAllocator" } );

			// Create the copy command list
			if ( FAILED( m_Device->CreateCommandList1( 0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS( &m_CopyCommandList ) ) ) )
			{
				return false;
			}
			D3D12_SET_DEBUG_NAME( m_CopyCommandList.Get(), StringView{ "RHICopyCmdList" } );

			// Create the copy fence
			m_CopyFence = SharedPtrCast<D3D12Fence>( CreateFence( RHIFenceDescriptor{ "RHICopyFence", ERHIFenceType::CPUWaitOnly } ) );
			if ( !m_CopyFence )
			{
				return false;
			}
		}


        return true;
    }

	bool D3D12RHI::Shutdown()
	{
		m_UploadBuffer.Release();

		// Release the copy objects
		m_CopyCommandList.Release();
		m_CopyAllocator.Release();
		m_CopyQueue.Release();
		m_CopyFence->Release();
		m_CopyFence.reset();

		m_CommandList.Release();
		m_CommandAllocator.Release();
		m_CommandQueue.Release();
		m_Device.Release();

		m_DXGIFactory.Release();

		m_Fence.reset();

    #if RHI_DEBUG_ENABLED
		DumpDebug();
		m_DXGIDebug.Release();
		m_D3D12Debug.Release();
    #endif

		return true;
	}

    bool D3D12RHI::ExecuteCommandList( RHICommandListRef a_CommandList )
    {
		ID3D12::GraphicsCommandList* cmdList = a_CommandList->As<D3D12CommandList>()->CommandList.Get();
		if ( FAILED( cmdList->Close() ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to close command list" );
			return false;
		}

        ID3D12CommandList* cmdLists[] = { cmdList };
        m_CommandQueue->ExecuteCommandLists( 1, cmdLists );
		m_CommandQueue->Signal( RHI::GetGlobalFence()->NativePtrAs<ID3D12Fence>(), ++s_RHIGlobals.FrameFenceValue );
		RHI::GetGlobalFence()->Wait( s_RHIGlobals.FrameFenceValue );
		return true;

		return true;
    }

    //////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHIFenceRef D3D12RHI::CreateFence( const RHIFenceDescriptor& a_Desc )
	{
		RHIFenceRef fence = RHIResource::Create<D3D12Fence>();
		CHECK( fence->Commit( a_Desc ) );
		return fence;
	}

	RHISamplerRef D3D12RHI::CreateSampler( const RHISamplerDescriptor& a_Desc )
	{
		RHISamplerRef sampler = RHIResource::Create<D3D12Sampler>();
		CHECK( sampler->Commit( a_Desc ) );
		return sampler;
	}

	RHITextureRef D3D12RHI::CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
	{
		RHITextureRef tex = RHIResource::Create<D3D12Texture>();
		return tex;
	}

	RHIBufferRef D3D12RHI::CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data )
	{
		RHIBufferRef buffer = RHIResource::Create<D3D12Buffer>();
		return buffer;
	}

	RHIGraphicsPipelineStateRef D3D12RHI::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc )
	{
		RHIGraphicsPipelineStateRef pipeline = RHIResource::Create<D3D12GraphicsPipelineState>();
		CHECK( pipeline->Commit( a_Desc ) );
		return pipeline;
	}

	RHICommandListRef D3D12RHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		RHICommandListRef cmdList = RHIResource::Create<D3D12CommandList>();
		CHECK( cmdList->Commit( a_Desc ) );
		return cmdList;
	}

	RHIShaderModuleRef D3D12RHI::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		RHIShaderModuleRef shader = RHIResource::Create<D3D12ShaderModule>();
		CHECK( shader->Commit( a_Desc ) );
		return shader;
	}

	RHIShaderBindingLayoutRef D3D12RHI::CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc )
	{
		RHIShaderBindingLayoutRef sbl = RHIResource::Create<D3D12ShaderBindingLayout>();
		CHECK( sbl->Commit( a_Desc ) );
		return sbl;
	}

	RHISwapChainRef D3D12RHI::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		RHISwapChainRef swapChain = RHIResource::Create<D3D12SwapChain>();
		CHECK( swapChain->Commit( a_Desc ) );
		return swapChain;
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
