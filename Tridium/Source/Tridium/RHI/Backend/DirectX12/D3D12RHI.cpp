#include "tripch.h"
#include "D3D12RHI.h"

// For getting the native window handle
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>


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

		if ( !m_WindowData.Initialize( *this ) )
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
		if ( !m_WindowData.SwapChain )
			return false;

		m_WindowData.SwapChain->Present( 1, 0 );
		return true;
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
        CHECK( tex->Commit( &a_Desc ) );
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

    bool DirectX12RHI::WindowData::Initialize( DirectX12RHI& a_RHI )
    {
		// Get the native window handle
		HWND hWnd = glfwGetWin32Window( glfwGetCurrentContext() );
		if ( hWnd == NULL )
		{
			return false;
		}
       
		// Create the swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.Width = 0;
		swapChainDesc.Height = 0;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = 0;
		ComPtr<IDXGISwapChain1> swapChain;
		if ( FAILED( a_RHI.m_DXGIFactory->CreateSwapChainForHwnd( a_RHI.m_CommandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain ) ) )
		{
			return false;
		}
		if ( !swapChain.QueryInterface( SwapChain ) )
		{
			return false;
		}

		// Create Render Target View Descriptor Heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = s_FrameCount;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		if ( FAILED( a_RHI.m_Device->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS( &RTVDescHeap ) ) ) )
		{
			return false;
		}

		// Create handles to view
		auto firstHandle = RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		auto handleIncrement = a_RHI.m_Device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
		for ( size_t i = 0; i < s_FrameCount; ++i )
		{
			RTVHandles[i] = firstHandle;
			RTVHandles[i].ptr += handleIncrement * i;
		}

		// Get the back buffers
		for ( uint32_t i = 0; i < 2; i++ )
		{
			if ( FAILED( SwapChain->GetBuffer( i, IID_PPV_ARGS( &Buffers[i] ) ) ) )
			{
				return false;
			}

			D3D12_RENDER_TARGET_VIEW_DESC rtv{};
			rtv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtv.Texture2D.MipSlice = 0;
			rtv.Texture2D.PlaneSlice = 0;
			a_RHI.m_Device->CreateRenderTargetView( Buffers[i], &rtv, RTVHandles[i] );
		}

		return true;
    }

} // namespace Tridium
