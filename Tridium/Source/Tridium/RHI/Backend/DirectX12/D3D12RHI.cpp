#include "tripch.h"
#include "D3D12RHI.h"

// Resources
#include <Tridium/RHI/RHICommandList.h>
#include <Tridium/RHI/RHITexture.h>
#include <Tridium/RHI/RHIMesh.h>
#include <Tridium/RHI/RHIPipelineState.h>

#include "D3D12Texture.h"
//#include "D3D12Mesh.h"
//#include "D3D12PipelineState.h"


namespace Tridium {

	//////////////////////////////////////////////////////////////////////////
	// CORE RHI FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

    bool DirectX12RHI::Init( const RHIConfig& a_Config )
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

	bool DirectX12RHI::Shutdown()
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

    bool DirectX12RHI::Present()
    {
		return false;
    }

    bool DirectX12RHI::ExecuteCommandList( RHICommandListRef a_CommandList )
    {
		if ( FAILED( m_CommandList->Close() ) )
		{
			return false;
		}

        ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
        m_CommandQueue->ExecuteCommandLists( 1, cmdLists );

		return false;
    }

	//////////////////////////////////////////////////////////////////////////
	// FENCE FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

    RHIFence DirectX12RHI::CreateFence() const
    {
        return RHIFence();
    }

    ERHIFenceState DirectX12RHI::GetFenceState( RHIFence a_Fence ) const
    {
        return ERHIFenceState();
    }

    void DirectX12RHI::FenceSignal( RHIFence a_Fence )
    {
    }

    //////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHITextureRef DirectX12RHI::CreateTexture( const RHITextureDescriptor& a_Desc )
	{
        // Create raw pointer first so compiler can optimize out dynamic dispatch
        D3D12Texture* tex = new D3D12Texture();
        tex->Commit( &a_Desc );
        return SharedPtr<RHITexture>( tex );
	}

	RHIIndexBufferRef DirectX12RHI::CreateIndexBuffer( const RHIIndexBufferDescriptor& a_Desc )
	{
		return nullptr;
	}

	RHIVertexBufferRef DirectX12RHI::CreateVertexBuffer( const RHIVertexBufferDescriptor& a_Desc )
	{
		return nullptr;
	}

	RHIPipelineStateRef DirectX12RHI::CreatePipelineState( const RHIPipelineStateDescriptor& a_Desc )
	{
		return nullptr;
	}

	RHICommandListRef DirectX12RHI::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
    // DEBUG
	//////////////////////////////////////////////////////////////////////////

#if RHI_DEBUG_ENABLED
    void DirectX12RHI::DumpDebug()
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
