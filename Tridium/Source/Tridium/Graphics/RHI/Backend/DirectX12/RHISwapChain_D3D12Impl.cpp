#include "tripch.h"
#include "RHI_D3D12Impl.h"

// For getting the native window handle
#include <GLFW/glfw3.h>
#if CONFIG_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>
#else
#error "Only Windows is supported for now"
#endif

namespace Tridium::D3D12 {

	RHISwapChain_D3D12Impl::RHISwapChain_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHISwapChain( a_Device, a_Desc )
	{
		HRESULT hr = S_OK;
		HWND hWnd = glfwGetWin32Window( glfwGetCurrentContext() );
		if ( !hWnd )
		{
			ASSERT( false, "Failed to get window handle while creating IRHISwapChain" );
			return;
		}

		// Create the swap chain descriptor
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.Width = m_Width = a_Desc.Width;
		swapChainDesc.Height = m_Height = a_Desc.Height;
		swapChainDesc.Format = D3D12::Translate( a_Desc.Format );
		TODO( "Stereo?" );
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = a_Desc.SampleSettings.Count;
		swapChainDesc.SampleDesc.Quality = a_Desc.SampleSettings.Quality;
		TODO( "Use RHIUsageHint for BufferUsage?" );
		swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = a_Desc.BufferCount;
		swapChainDesc.Scaling =
			a_Desc.ScaleMode == ERHIScaleMode::Stretch
			? DXGI_SCALING_STRETCH
			: a_Desc.ScaleMode == ERHIScaleMode::AspectRatioStretch
			? DXGI_SCALING_ASPECT_RATIO_STRETCH
			: DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = 0;
		swapChainDesc.Flags |= a_Desc.Flags.HasFlag( ERHISwapChainFlags::UseVSync ) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		TODO( "this?" );
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
		fsDesc.Windowed = true;

		// Create the swap chain
		ComPtr<IDXGISwapChain1> swapChain;
		ComPtr<IDXGIFactory4> dxgiFactory;
		hr = Device()->GetDXGIFactory()->QueryInterface( IID_PPV_ARGS( dxgiFactory.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			ASSERT( false, "Failed to query DXGI factory interface!" );
			return;
		}


		CommandQueue* cmdQueue = Device()->GetCommandQueue( ERHICommandQueueType::Graphics );
		hr = dxgiFactory->CreateSwapChainForHwnd( cmdQueue->CmdQueue.Get(), hWnd, &swapChainDesc, &fsDesc, nullptr, &swapChain );
		if ( FAILED( hr ) )
		{
			ASSERT( false, "Failed to create swap chain!" );
			return;
		}

		hr = swapChain->QueryInterface( SwapChain.GetAddressOf() );
		if ( FAILED( hr ) )
		{
			ASSERT( false, "Failed to query swap chain interface!" );
			return;
		}

		// Resize the RTVs array to the buffer count
		RTVs.Resize( a_Desc.BufferCount );

		ASSERT( GetBackBuffers(), "Failed to get back buffers!" );
	}

	bool RHISwapChain_D3D12Impl::Present()
	{
		if ( !SwapChain )
			return false;

		RHI_DEV_CHECK( GetBackBuffer()->State() == ERHIResourceStates::Present,
			"Back buffer state is not 'Present'! - You should be setting a resource barrier to transition it to Present state before presenting!" );

		SwapChain->Present( 1, 0 );
		CommandQueue* cmdQueue = Device()->GetCommandQueue( ERHICommandQueueType::Graphics );
		cmdQueue->CmdQueue->Signal( cmdQueue->Fence.Get(), ++cmdQueue->LastSubmittedValue );
		m_LastPresentedValue = cmdQueue->LastSubmittedValue;


		if ( m_ShouldResize && !ResizeBuffers() )
		{
			return false;
		}

		return true;
	}

	RHITextureRef RHISwapChain_D3D12Impl::GetBackBuffer()
	{
		if ( !SwapChain )
			return nullptr;
		return RTVs[SwapChain->GetCurrentBackBufferIndex()];
	}

	bool RHISwapChain_D3D12Impl::Resize( uint32_t a_Width, uint32_t a_Height )
	{
		if ( !SwapChain )
		{
			return false;
		}

		if ( a_Width == m_Width && a_Height == m_Height )
		{
			return true;
		}

		m_Width = Math::Max( a_Width, 8u );
		m_Height = Math::Max( a_Height, 8u );

		m_ShouldResize = true;

		return true;
	}

	bool RHISwapChain_D3D12Impl::ResizeBuffers()
	{
		ReleaseBuffers();

		// Resize the swap chain
		if ( FAILED( SwapChain->ResizeBuffers( RTVs.Size(), m_Width, m_Height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to resize swap chain buffers!" );
			return false;
		}

		if ( !GetBackBuffers() )
		{
			LOG( LogCategory::DirectX, Error, "Failed to get back buffers!" );
			return false;
		}

		m_ShouldResize = false;
		return true;
	}

	void RHISwapChain_D3D12Impl::ReleaseBuffers()
	{
		for ( uint32_t i = 0; i < RTVs.Size(); i++ )
		{
			if ( RTVs[i] != nullptr )
			{
				RHI_DEV_CHECK( RTVs[i].use_count() == 1, "RTV owned by the swap chain is still in use - You should not be keeping a reference to the back buffer!" );
				RTVs[i] = nullptr;
			}
		}
	}

	bool RHISwapChain_D3D12Impl::GetBackBuffers()
	{
		// Create textures and handles to view
		const auto rtvDesc =
			RHITextureDesc{}
			.SetFormat( Desc().Format )
			.SetWidth( m_Width )
			.SetHeight( m_Height )
			.SetDimension( ERHITextureDimension::Texture2D )
			.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource );

		for ( uint32_t i = 0; i < RTVs.Size(); i++ )
		{
			if ( RTVs[i] == nullptr )
			{
				RHITextureDesc desc = rtvDesc;
				RHI_DEBUG_OP( desc.Name = std::format( "{} RTV[{}]", Desc().Name, i ) );
				RTVs[i] = RHI::CreateTexture( desc );
			}

			TODO( "This" );
			{
				// Extreme jankiness here. RHI::CreateTexture creates a D3D12MA with its own resource.
				// We don't need that resource. So we get the swap chain buffer and set it as the resource for the texture.
				// And GetBuffer() increments the ref and so does SetResource(), so we need to release it once.

				RHITexture_D3D12Impl* tex = RTVs[i]->As<RHITexture_D3D12Impl>();
				ID3D12Resource* resource = nullptr;
				if ( FAILED( SwapChain->GetBuffer( i, IID_PPV_ARGS( &resource ) ) ) )
				{
					ASSERT( false, "Failed to get back buffer!" );
					return false;
				}
				resource->Release();
				resource->SetName( L"SwapChainBackBuffer" );
				tex->Texture.Allocation->SetResource( resource );
			}
		}

		return true;
	}

	bool RHISwapChain_D3D12Impl::Release()
	{
		Device()->WaitForFence( ERHICommandQueueType::Graphics, m_LastPresentedValue );

		for ( uint32_t i = 0; i < RTVs.Size(); i++ )
		{
			if ( RTVs[i] != nullptr )
			{
				RHI_DEV_CHECK( RTVs[i].use_count() == 1, "RTV owned by the swap chain is still in use - You should not be keeping a reference to the back buffer!" );
				RTVs[i] = nullptr;
			}
		}

		if ( SwapChain )
			SwapChain.Reset();

		return true;
	}

	bool RHISwapChain_D3D12Impl::Valid() const
	{
		return SwapChain != nullptr;
	}
}
