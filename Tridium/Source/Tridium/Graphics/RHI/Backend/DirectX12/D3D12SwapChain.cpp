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
		return true;
	}

	RHITextureRef D3D12SwapChain::GetBackBuffer()
	{
		if ( !SwapChain )
			return nullptr;
		return RTVs[SwapChain->GetCurrentBackBufferIndex()].second;
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
		swapChainDesc.Width = desc->Width;
		swapChainDesc.Height = desc->Height;
		swapChainDesc.Format = ToD3D12::GetFormat( desc->Format );
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

		// Create Render Target View Descriptor Heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = desc->BufferCount;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		if ( FAILED( rhi->GetDevice()->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS( &RTVDescHeap ) ) ) )
		{
			return false;
		}

		// Resize the RTVs array to the buffer count
		RTVs.Resize( desc->BufferCount );

		// Create textures and handles to view
		RHITextureDescriptor rtvDesc;
		rtvDesc.Format = desc->Format;
		rtvDesc.Width = desc->Width;
		rtvDesc.Height = desc->Height;
		auto firstHandle = RTVDescHeap->GetCPUDescriptorHandleForHeapStart();
		auto handleIncrement = rhi->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for ( size_t i = 0; i < desc->BufferCount; ++i )
		{
			RTVs[i].first = firstHandle;
			RTVs[i].first.ptr += handleIncrement * i;

			SharedPtr<D3D12Texture> rtv = MakeShared<D3D12Texture>();
			rtv->DescriptorHandle = RTVs[i].first;
			rtv->Descriptor = MakeUnique<RHITextureDescriptor>( rtvDesc );
			RTVs[i].second = rtv;
		}

		// Get the back buffers
		for ( uint32_t i = 0; i < 2; i++ )
		{
			SharedPtr<D3D12Texture> tex = RTVs[i].second->As<D3D12Texture>();
			if ( tex == nullptr || FAILED( SwapChain->GetBuffer( i, IID_PPV_ARGS( &tex->Texture ) ) ) )
			{
				return false;
			}

			D3D12_RENDER_TARGET_VIEW_DESC rtv{};
			rtv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtv.Texture2D.MipSlice = 0;
			rtv.Texture2D.PlaneSlice = 0;
			rhi->GetDevice()->CreateRenderTargetView( tex->Texture, &rtv, RTVs[i].first );

		#if RHI_DEBUG_ENABLED
			if ( RHIQuery::IsDebug() )
			{
				WString name = desc->Name.empty() ? L"SwapChain" : ToD3D12::ToWString( desc->Name.data() );
				name += L" RTV[" + std::to_wstring( i ) + L"]";
				tex->Texture->SetName( name.c_str() );
				D3D12Context::Get()->StringStorage.EmplaceBack( std::move( name ) );
			}
		#endif
		}

		return true;
    }

	bool D3D12SwapChain::Release()
	{
		SwapChain.Release();
		RTVDescHeap.Release();
		for ( auto& rtv : RTVs )
		{
			rtv = { D3D12_CPU_DESCRIPTOR_HANDLE{}, nullptr };
		}
		RTVs.Resize( 0 );
		return true;
	}

	bool D3D12SwapChain::IsValid() const
	{
		return SwapChain != nullptr;
	}
}
