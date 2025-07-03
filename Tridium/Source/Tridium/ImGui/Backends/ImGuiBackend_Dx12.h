#pragma once
#include <Tridium/ImGui/ImGuiBackend.h>
#include <backends/imgui_impl_dx12.cpp>
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Graphics/RHI/Backend/DirectX12/RHI_D3D12Impl.h>

namespace Tridium::D3D12 {

	class RendererImGuiInterface_D3D12 : public IRendererImGuiInterface
	{
	public:
		bool Init( IDynamicRHI* a_RHI ) override
		{
			DynamicRHI_D3D12Impl* rhi = DynamicCast<DynamicRHI_D3D12Impl*>( a_RHI );
			ENSURE( rhi != nullptr, "RHI is null!" );

			CBVSRVHeap = rhi->GetDescriptorHeapManager().AllocateIndependentHeap(
				ERHIDescriptorHeapType::RenderResource, 
				1024, // Number of descriptors 
				EDescriptorHeapFlags::GPUVisible, 
				"ImGui CBV/SRV Heap" 
			);
			ENSURE( CBVSRVHeap != nullptr, "Failed to create CBV/SRV heap!" );

			RTVHeap = rhi->GetDescriptorHeapManager().AllocateIndependentHeap(
				ERHIDescriptorHeapType::RenderTarget,
				RHIConstants::MaxColorTargets,
				EDescriptorHeapFlags::None,
				"ImGui RTV Heap"
			);
			ENSURE( RTVHeap != nullptr, "Failed to create RTV heap!" );

			ID3D12Device* device = rhi->GetD3D12Device();
			int numFramesInFlight = rhi->Config().MaxFramesInFlight;
			DXGI_FORMAT rtvFormat = Translate( rhi->GetSwapChain()->Desc().Format );
			ID3D12DescriptorHeap* cbvSrvHeap = CBVSRVHeap->Heap();

			if ( !ImGui_ImplDX12_Init(
				device, numFramesInFlight, rtvFormat, cbvSrvHeap,
				CBVSRVHeap->GetCPUHandle( 0 ), CBVSRVHeap->GetGPUHandle( 0 )
			) )
			{
				ENSURE( false, "Failed to initialize ImGui OpenGL backend!" );
				return false;
			}

			CmdContext.QueueType = ERHICommandQueueType::Graphics;
			device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( CmdContext.CmdAllocator.GetAddressOf() ) );
			ENSURE( CmdContext.CmdAllocator != nullptr, "Failed to create command allocator!" );

			device->CreateCommandList(
				0, D3D12_COMMAND_LIST_TYPE_DIRECT, CmdContext.CmdAllocator.Get(), nullptr,
				IID_PPV_ARGS( CmdContext.CmdList.GetAddressOf() )
			);
			ENSURE( CmdContext.CmdList != nullptr, "Failed to create command list!" );

			device->CreateFence(
				0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( CmdContext.Fence.GetAddressOf() )
			);
			ENSURE( CmdContext.Fence != nullptr, "Failed to create fence!" );

			return true;
		}

		void Shutdown() override
		{
			ImGui_ImplDX12_Shutdown();
			CmdContext = {};
			CBVSRVHeap.reset();
			RTVHeap.reset();
		}

		void NewFrame() override
		{
			ImGui_ImplDX12_NewFrame();
		}

		void RenderDrawData( ImDrawData* a_DrawData, const RHICommandListRef& a_CmdList, const RHITextureRef& a_RenderTarget ) override
		{
			// Create RTV for the render target
			if ( !RTVHeap || !a_RenderTarget )
			{
				ENSURE( false, "RTV heap or render target is null!" );
				return;
			}

			auto* texture = a_RenderTarget->As<RHITexture_D3D12Impl>();
			if ( !texture || !texture->Texture.Resource() )
			{
				ENSURE( false, "Invalid render target texture!" );
				return;
			}

			ID3D12Device* device = GetD3D12RHI()->GetD3D12Device();
			device->CreateRenderTargetView(
				texture->Texture.Resource(), nullptr, RTVHeap->GetCPUHandle( 0 )
			);

			auto* cmdList = Cast<ID3D12GraphicsCommandList*>( CmdContext.CmdList.Get() );
			constexpr FLOAT clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
			auto* heap = CBVSRVHeap->Heap();

			CmdContext.CmdAllocator->Reset();
			cmdList->Reset( CmdContext.CmdAllocator.Get(), nullptr );

			D3D12_RESOURCE_BARRIER barrier = Translate( RHIResourceBarrier{ 
				.Resource = a_RenderTarget.get(),
				.Before = a_RenderTarget->State(),
				.After = ERHIResourceStates::RenderTarget 
			} );
			cmdList->ResourceBarrier( 1, &barrier );
			auto rtvCpuHandle = RTVHeap->GetCPUHandle( 0 );
			cmdList->OMSetRenderTargets( 1, &rtvCpuHandle, FALSE, nullptr );
			cmdList->ClearRenderTargetView( rtvCpuHandle, clearColor, 0, nullptr );
			cmdList->SetDescriptorHeaps( 1, &heap );

			ImGui_ImplDX12_RenderDrawData( a_DrawData, cmdList );

			barrier = Translate( RHIResourceBarrier{
				.Resource = a_RenderTarget.get(),
				.Before = ERHIResourceStates::RenderTarget,
				.After = a_RenderTarget->State()
			} );
			cmdList->ResourceBarrier( 1, &barrier );

			cmdList->Close();
			auto* cmdQueue = GetD3D12RHI()->GetCommandQueue( ERHICommandQueueType::Graphics );
			cmdQueue->CmdQueue->ExecuteCommandLists(
				1, CmdContext.CmdList.GetAddressOf()
			);

			GetD3D12RHI()->WaitForFence( ERHICommandQueueType::Graphics, cmdQueue->Signal() );
		}

		SharedPtr<DescriptorHeap> CBVSRVHeap;
		SharedPtr<DescriptorHeap> RTVHeap;
		CommandContext CmdContext;
	};

} // namespace Tridium