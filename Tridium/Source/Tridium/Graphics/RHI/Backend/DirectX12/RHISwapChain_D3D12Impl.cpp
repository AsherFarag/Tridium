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

	RHISwapChain_D3D12Impl::RHISwapChain_D3D12Impl( const DescriptorType& a_Desc )
		: RHISwapChain( a_Desc )
	{
		DynamicRHI_D3D12Impl* rhi = GetD3D12RHI();

		HWND hWnd = glfwGetWin32Window( glfwGetCurrentContext() );
		if ( !ASSERT( hWnd != NULL,
			"Failed to get window handle while creating RHISwapChain" ) )
		{
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

		auto& directCmdCtx = rhi->GetCommandContext( ERHICommandQueueType::Graphics );

		// Create the swap chain
		ComPtr<IDXGISwapChain1> swapChain;
		ComPtr<IDXGIFactory4> dxgiFactory;
		if ( !ASSERT( SUCCEEDED( rhi->GetDXGIFactory()->QueryInterface( dxgiFactory.GetAddressOf() ) ),
			"Failed to get DXGI factory!" ) )
		{
			return;
		}

		if ( !ASSERT(
			SUCCEEDED( dxgiFactory->CreateSwapChainForHwnd( directCmdCtx.CmdQueue.Get(), hWnd, &swapChainDesc, &fsDesc, nullptr, &swapChain ) ),
			"Failed to create swap chain!" ) )
		{
			return;
		}

		if ( !ASSERT( SUCCEEDED( swapChain->QueryInterface( SwapChain.GetAddressOf() ) ),
			"Failed to query swap chain interface!" ) )
		{
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

		if ( GetBackBuffer()->State() != ERHIResourceStates::Present )
		{
			TODO( "We should not be stalling the GPU here!" );
			// We need to transition the back buffer to present
			auto& cmdCtx = GetD3D12RHI()->GetCommandContext( ERHICommandQueueType::Graphics );
			cmdCtx.Wait( cmdCtx.Signal() );
		}

		SwapChain->Present( 1, 0 );

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
		// Wait for the GPU to finish
		auto& cmdCtx = GetD3D12RHI()->GetCommandContext( ERHICommandQueueType::Graphics );
		cmdCtx.Wait( cmdCtx.Signal() );

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
			ASSERT( RTVs[i].use_count() == 1, "RTV owned by the swap chain is still in use - You should not be keeping a reference to the back buffer!" );
			RTVs[i]->Release();
			RTVs[i] = nullptr;
		}
	}

	bool RHISwapChain_D3D12Impl::GetBackBuffers()
	{
		// Create textures and handles to view
		const auto rtvDesc =
			RHITextureDescriptor{}
			.SetFormat( Descriptor().Format )
			.SetWidth( m_Width )
			.SetHeight( m_Height )
			.SetDimension( ERHITextureDimension::Texture2D )
			.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource );

		for ( uint32_t i = 0; i < RTVs.Size(); i++ )
		{
			RHITextureDescriptor desc = rtvDesc;
		#if RHI_DEBUG_ENABLED
			desc.Name = std::format( "{} RTV[{}]", Descriptor().Name, i );
		#endif

			if ( !RTVs[i] )
			{
				RTVs[i] = RHI::CreateTexture( desc );
			}

			RHITexture_D3D12Impl* tex = RTVs[i]->As<RHITexture_D3D12Impl>();
			if ( FAILED( SwapChain->GetBuffer( i, IID_PPV_ARGS( &tex->Texture.Resource ) ) ) )
			{
				ASSERT( false, "Failed to get back buffer!" );
				return false;
			}
		}

		return true;
	}

	bool RHISwapChain_D3D12Impl::Release()
	{
		SwapChain.Reset();
		for ( auto& rtv : RTVs )
		{
			ASSERT( rtv.use_count() == 1, "RTV owned by the swap chain is still in use - You should not be keeping a reference to the back buffer!" );
			TODO( "Hack as for some reason the ID3D12Resource's have one extra ref. Figure this out." );
			if ( ID3D12Resource* resource = rtv->As<RHITexture_D3D12Impl>()->Texture.Allocation->GetResource() )
			{
				resource->Release();
			}
			rtv->Release();
			rtv = nullptr;
		}
		return true;
	}

	bool RHISwapChain_D3D12Impl::Valid() const
	{
		return SwapChain != nullptr;
	}
}
