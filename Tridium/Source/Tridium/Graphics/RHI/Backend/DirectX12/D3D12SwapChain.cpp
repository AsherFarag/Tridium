#include "tripch.h"
#include "D3D12SwapChain.h"
#include "D3D12DynamicRHI.h"
#include "D3D12Texture.h"

// For getting the native window handle
#include <GLFW/glfw3.h>
#if CONFIG_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>
#else
#error "Only Windows is supported for now"
#endif

namespace Tridium {

	bool D3D12SwapChain::Present()
	{
		if ( !SwapChain )
			return false;

		if ( GetBackBuffer()->GetState() != ERHIResourceStates::Present )
		{
			TODO( "We should not be stalling the GPU here!" );
			// We need to transition the back buffer to present
			auto& cmdCtx = GetD3D12RHI()->GetCommandContext( ERHICommandQueueType::Graphics );
			cmdCtx.Wait( cmdCtx.Signal() );
		}

		SwapChain->Present( 1, 0 );

		if ( m_ShouldResize && !ResizeBuffers() )
		{
			ASSERT_LOG( false, "Failed to resize buffers" );
			return false;
		}

		return true;
	}

	RHITextureRef D3D12SwapChain::GetBackBuffer()
	{
		if ( !SwapChain )
			return nullptr;
		return RTVs[SwapChain->GetCurrentBackBufferIndex()];
	}

	bool D3D12SwapChain::Resize( uint32_t a_Width, uint32_t a_Height )
	{
		if ( !SwapChain )
		{
			return false;
		}

		if ( a_Width == m_Width && a_Height == m_Height )
		{
			return true;
		}

		m_Width = a_Width;
		m_Height = a_Height;

		m_ShouldResize = true;

		return true;
	}

	bool D3D12SwapChain::ResizeBuffers()
	{
		// Wait for the GPU to finish
		auto& cmdCtx = GetD3D12RHI()->GetCommandContext( ERHICommandQueueType::Graphics );
		cmdCtx.Wait( cmdCtx.Signal() );

		ReleaseBuffers();

		// Resize the swap chain
		if ( FAILED( SwapChain->ResizeBuffers( RTVs.Size(), m_Width, m_Height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING ) ) )
		{
			ASSERT_LOG( false, "Failed to resize swap chain buffers!" );
			return false;
		}

		if ( !GetBackBuffers() )
		{
			ASSERT_LOG( false, "Failed to get back buffers!" );
			return false;
		}

		m_ShouldResize = false;
		return true;
	}

	void D3D12SwapChain::ReleaseBuffers()
	{
		for ( uint32_t i = 0; i < RTVs.Size(); i++ )
		{
			ASSERT_LOG( RTVs[i].use_count() == 1, "RTV owned by the swap chain is still in use - You should not be keeping a reference to the back buffer!" );
			RTVs[i]->Release();
			RTVs[i] = nullptr;
		}
	}

	bool D3D12SwapChain::GetBackBuffers()
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
			if ( !RTVs[i] )
			{
				RTVs[i] = RHI::CreateTexture( rtvDesc );
			}

			D3D12Texture* tex = RTVs[i]->As<D3D12Texture>();
			if ( FAILED( SwapChain->GetBuffer( i, IID_PPV_ARGS( &tex->Texture.Resource ) ) ) )
			{
				ASSERT_LOG( false, "Failed to get back buffer!" );
				return false;
			}

		#if RHI_DEBUG_ENABLED
			if ( RHIQuery::IsDebug() )
			{
				WString name = Descriptor().Name.empty() ? L"SwapChain" : WString( Descriptor().Name.begin(), Descriptor().Name.end() );
				name += L" RTV[" + std::to_wstring( i ) + L"]";
				tex->Texture.Resource.Get()->SetName( name.c_str() );
				D3D12Context::Get()->StringStorage.EmplaceBack( std::move( name ) );
			}
		#endif
		}

		return true;
	}

	bool D3D12SwapChain::Commit( const RHISwapChainDescriptor& a_Desc )
    {
		m_Desc = a_Desc;

		// Get the RHI
		D3D12RHI* rhi = GetD3D12RHI();


		// Get the native window handle
		HWND hWnd = glfwGetWin32Window( glfwGetCurrentContext() );
		if ( hWnd == NULL )
		{
			return false;
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
		if ( !rhi->GetDXGIFactory().QueryInterface( dxgiFactory ) )
		{
			ASSERT_LOG( false, "Failed to get DXGI factory!" );
			return false;
		}

		if ( FAILED( dxgiFactory->CreateSwapChainForHwnd( directCmdCtx.CmdQueue.Get(), hWnd, &swapChainDesc, &fsDesc, nullptr, &swapChain) ) )
		{
			ASSERT_LOG( false, "Failed to create swap chain!" );
			return false;
		}

		if ( !swapChain.QueryInterface( SwapChain ) )
		{
			return false;
		}

		// Resize the RTVs array to the buffer count
		RTVs.Resize( a_Desc.BufferCount );

		// Get the back buffers
		if ( !GetBackBuffers() )
		{
			return false;
		}

		return true;
    }

	bool D3D12SwapChain::Release()
	{
		SwapChain.Release();
		for ( auto& rtv : RTVs )
		{
			rtv = nullptr;
		}
		RTVs.Resize( 0 );
		return true;
	}

	bool D3D12SwapChain::IsValid() const
	{
		return SwapChain != nullptr;
	}
}
