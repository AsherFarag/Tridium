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
		RHI::GetD3D12RHI()->FenceSignal( RHI::GetD3D12RHI()->CreateFence() );

		ReleaseBuffers();

		// Resize the swap chain
		if ( FAILED( SwapChain->ResizeBuffers( RTVs.Size(), m_Width, m_Height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING ) ) )
		{
			return false;
		}

		if ( !GetBackBuffers() )
		{
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
		RHITextureDescriptor rtvDesc;
		rtvDesc.Format = GetDescriptor()->Format;
		rtvDesc.Width = GetDescriptor()->Width;
		rtvDesc.Height = GetDescriptor()->Height;
		rtvDesc.Depth = 1;
		rtvDesc.Layers = 1;
		rtvDesc.Mips = 1;
		rtvDesc.UsageHint = ERHIUsageHint::RenderTarget;
		rtvDesc.Dimension = ERHITextureDimension::Texture2D;
		rtvDesc.IsRenderTarget = true;

		for ( uint32_t i = 0; i < RTVs.Size(); i++ )
		{
			if ( !RTVs[i] )
			{
				RTVs[i] = RHI::GetD3D12RHI()->CreateTexture( rtvDesc );
			}

			D3D12Texture* tex = RTVs[i]->As<D3D12Texture>();
			if ( FAILED( SwapChain->GetBuffer( i, IID_PPV_ARGS( &tex->Texture.Resource ) ) ) )
			{
				return false;
			}

		#if RHI_DEBUG_ENABLED
			if ( RHIQuery::IsDebug() )
			{
				WString name = GetDescriptor()->Name.empty() ? L"SwapChain" : WString( GetDescriptor()->Name.begin(), GetDescriptor()->Name.end() );
				name += L" RTV[" + std::to_wstring( i ) + L"]";
				tex->Texture.Resource.Get()->SetName( name.c_str() );
				D3D12Context::Get()->StringStorage.EmplaceBack( std::move( name ) );
			}
		#endif
		}

		return true;
	}

	bool D3D12SwapChain::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHISwapChainDescriptor>( a_Params );
		if ( !desc )
		{
			return false;
		}

		// Get the RHI
		D3D12RHI* rhi = RHI::GetD3D12RHI();


		// Get the native window handle
		HWND hWnd = glfwGetWin32Window( glfwGetCurrentContext() );
		if ( hWnd == NULL )
		{
			return false;
		}

		// Create the swap chain descriptor
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.Width = m_Width = desc->Width;
		swapChainDesc.Height = m_Height = desc->Height;
		swapChainDesc.Format = D3D12::Translate( desc->Format );
		TODO( "Stereo?" );
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = desc->SampleSettings.Count;
		swapChainDesc.SampleDesc.Quality = desc->SampleSettings.Quality;
		TODO( "Use RHIUsageHint for BufferUsage?" );
		swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = desc->BufferCount;
		swapChainDesc.Scaling = 
			desc->ScaleMode == ERHIScaleMode::Stretch
			? DXGI_SCALING_STRETCH
			: desc->ScaleMode == ERHIScaleMode::AspectRatioStretch
				? DXGI_SCALING_ASPECT_RATIO_STRETCH
				: DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = 0;
		swapChainDesc.Flags |= desc->Flags.HasFlag( ERHISwapChainFlags::UseVSync ) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		TODO( "this?" );
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc{};
		fsDesc.Windowed = true;

		// Create the swap chain
		ComPtr<IDXGISwapChain1> swapChain;
		if ( FAILED( rhi->GetFactory()->CreateSwapChainForHwnd( rhi->GetCommandQueue().Get(), hWnd, &swapChainDesc, &fsDesc, nullptr, &swapChain)) )
		{
			return false;
		}

		if ( !swapChain.QueryInterface( SwapChain ) )
		{
			return false;
		}

		// Resize the RTVs array to the buffer count
		RTVs.Resize( desc->BufferCount );

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
