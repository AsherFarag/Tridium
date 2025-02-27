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

#include "D3D12Texture.h"
#include "D3D12Shader.h"
#include "D3D12Mesh.h"
#include "D3D12PipelineState.h"
#include "D3D12ShaderBindingLayout.h"
#include "D3D12CommandList.h"
#include "D3D12SwapChain.h"

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

		// Create the fence
        if ( FAILED( m_Device->CreateFence( m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_Fence ) ) ) )
        {
            return false;
        }

		// Create an event handle to use for frame synchronization
        m_FenceEvent = CreateEvent( nullptr, false, false, nullptr );
        if ( !m_FenceEvent )
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

        return true;
    }

	bool D3D12RHI::Shutdown()
	{
		m_CommandList.Release();
		m_CommandAllocator.Release();
		if ( m_FenceEvent )
		{
			CloseHandle( m_FenceEvent );
			m_FenceEvent = nullptr;
		}

		m_Fence.Release();
		m_CommandQueue.Release();
		m_Device.Release();

		m_DXGIFactory.Release();

    #if RHI_DEBUG_ENABLED
		m_DXGIDebug.Release();
		m_D3D12Debug.Release();
    #endif

		return true;
	}

    bool D3D12RHI::ExecuteCommandList( RHICommandListRef a_CommandList )
    {
		D3D12::GraphicsCommandList* cmdList = a_CommandList->As<D3D12CommandList>()->CommandList.Get();
		if ( FAILED( cmdList->Close() ) )
		{
			return false;
		}

        ID3D12CommandList* cmdLists[] = { cmdList };
        m_CommandQueue->ExecuteCommandLists( 1, cmdLists );
		return true;
    }

	//////////////////////////////////////////////////////////////////////////
	// FENCE FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

    RHIFence D3D12RHI::CreateFence() const
    {
        return RHIFence();
    }

    ERHIFenceState D3D12RHI::GetFenceState( RHIFence a_Fence ) const
    {
        return ERHIFenceState();
    }

    void D3D12RHI::FenceSignal( RHIFence a_Fence )
    {
		m_CommandQueue->Signal( m_Fence, ++m_FenceValue );
		if ( SUCCEEDED( m_Fence->SetEventOnCompletion( m_FenceValue, m_FenceEvent ) ) )
		{
			TODO( "Wtf is this?" );
			if ( WaitForSingleObject( m_FenceEvent, 20000 ) != WAIT_OBJECT_0 )
			{
				std::exit( 1 );
			}
		}
		else
		{
			std::exit( 1 );
		}
    }

    //////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHITextureRef D3D12RHI::CreateTexture( const RHITextureDescriptor& a_Desc )
	{
		RHITextureRef tex = RHIResource::Create<D3D12Texture>();
		CHECK( tex->Commit( &a_Desc ) );
		return tex;
	}

	RHIIndexBufferRef D3D12RHI::CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc )
	{
		RHIIndexBufferRef index = RHIResource::Create<D3D12IndexBuffer>();
		CHECK( index->Commit( &a_Desc ) );
		return index;
	}

	RHIVertexBufferRef D3D12RHI::CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc )
	{
		RHIVertexBufferRef vertex = RHIResource::Create<D3D12VertexBuffer>();
		CHECK( vertex->Commit( &a_Desc ) );
		return vertex;
	}

	RHIPipelineStateRef D3D12RHI::CreatePipelineState( const RHIPipelineStateDescriptor& a_Desc )
	{
		RHIPipelineStateRef pipeline = RHIResource::Create<D3D12PipelineState>();
		CHECK( pipeline->Commit( &a_Desc ) );
		return pipeline;
	}

	RHICommandListRef D3D12RHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		RHICommandListRef cmdList = RHIResource::Create<D3D12CommandList>();
		CHECK( cmdList->Commit( &a_Desc ) );
		return cmdList;
	}

	RHIShaderModuleRef D3D12RHI::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		RHIShaderModuleRef shader = RHIResource::Create<D3D12ShaderModule>();
		CHECK( shader->Commit( &a_Desc ) );
		return shader;
	}

	RHIShaderBindingLayoutRef D3D12RHI::CreateShaderBindingLayout( const RHIShaderBindingLayoutDescriptor& a_Desc )
	{
		RHIShaderBindingLayoutRef sbl = RHIResource::Create<D3D12ShaderBindingLayout>();
		CHECK( sbl->Commit( &a_Desc ) );
		return sbl;
	}

	RHISwapChainRef D3D12RHI::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		RHISwapChainRef swapChain = RHIResource::Create<D3D12SwapChain>();
		CHECK( swapChain->Commit( &a_Desc ) );
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
        }
    }
#endif // RHI_DEBUG_ENABLED

} // namespace Tridium
