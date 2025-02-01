#include "tripch.h"
#include "DX12RHI.h"

namespace Tridium::DX12 {

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
