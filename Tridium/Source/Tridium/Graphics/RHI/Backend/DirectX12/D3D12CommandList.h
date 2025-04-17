#pragma once
#include "D3D12Common.h"
#include "D3D12DescriptorHeap.h"
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHIStateTracker.h>
#include "D3D12PipelineState.h"

namespace Tridium {

	//=========================================================
	// D3D12 Command List
	//  DirectX 12 implementation of the RHICommandList interface.
	DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12CommandList, RHICommandList )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12CommandList, ERHInterfaceType::DirectX12 );

		D3D12CommandList( const RHICommandListDescriptor & a_Desc );
		virtual bool Release() override;
		virtual bool IsValid() const override { return CommandList != nullptr; }
		virtual const void* NativePtr() const override { return CommandList.Get(); }

		virtual bool SetGraphicsCommands( const RHIGraphicsCommandBuffer& a_CmdBuffer ) override;
		virtual bool SetComputeCommands( const RHIComputeCommandBuffer& a_CmdBuffer ) override;
		virtual bool IsCompleted() const override;
		virtual void WaitUntilCompleted() override;

		ID3D12GraphicsCommandList* GraphicsCommandList() const
		{
			RHI_DEV_CHECK( m_Desc.QueueType == ERHICommandQueueType::Graphics, "Command list is not a graphics command list!" );
			return static_cast<ID3D12GraphicsCommandList*>(CommandList.Get());
		}

		ComPtr<ID3D12CommandList> CommandList{};
		RHIShaderBindingLayout* CurrentSBL = nullptr;

	private:
		RHIResourceStateTracker m_ResourceStateTracker{};
		// Reset after each execution
		struct State
		{
			Array<D3D12::ManagedResource> D3D12Resources{};
			Array<D3D12_RESOURCE_BARRIER> D3D12Barriers{};
			Array<D3D12::DescriptorHeapRef> Heaps{};
			D3D12::DescriptorHeapRef LastRTVHeap{};
			D3D12::DescriptorHeapRef LastDSVHeap{};
			uint32_t ShaderInputOffset = 0;

			struct GraphicsState
			{
				D3D12GraphicsPipelineStateRef PSO{};
				Array<RHITexture*> CurrentRTs{};
				RHITexture* CurrentDSV{};
			} Graphics{};

			struct ComputeState
			{
			} Compute{};

			void Clear()
			{
				D3D12Resources.Clear();
				D3D12Barriers.Clear();
				Heaps.Clear();
				LastRTVHeap = nullptr;
				LastDSVHeap = nullptr;
				ShaderInputOffset = 0;
				Graphics.PSO = nullptr;
			}
		} m_State{};

		uint64_t m_FenceValue = 0;

	private:
		void CommitBarriers();

		// Commands

		void SetShaderBindingLayout( const RHICommand::SetShaderBindingLayout& a_Data );
		void SetShaderInput( const RHICommand::SetShaderInput& a_Data );
		void ResourceBarrier( const RHICommand::ResourceBarrier& a_Data );
		void UpdateBuffer( const RHICommand::UpdateBuffer& a_Data );
		void CopyBuffer( const RHICommand::CopyBuffer& a_Data );
		void UpdateTexture( const RHICommand::UpdateTexture& a_Data );
		void CopyTexture( const RHICommand::CopyTexture& a_Data );

		// Graphics
		void SetGraphicsPipelineState( const RHICommand::SetGraphicsPipelineState& a_Data );
		void SetRenderTargets( const RHICommand::SetRenderTargets& a_Data );
		void ClearRenderTargets( const RHICommand::ClearRenderTargets& a_Data );
		void SetScissors( const RHICommand::SetScissors& a_Data );
		void SetViewports( const RHICommand::SetViewports& a_Data );
		void SetIndexBuffer( const RHICommand::SetIndexBuffer& a_Data );
		void SetVertexBuffer( const RHICommand::SetVertexBuffer& a_Data );
		void SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology& a_Data );
		void Draw( const RHICommand::Draw& a_Data );
		void DrawIndexed( const RHICommand::DrawIndexed& a_Data );

		// Compute
		void SetComputePipelineState( const RHICommand::SetComputePipelineState& a_Data );
		void DispatchCompute( const RHICommand::DispatchCompute& a_Data );
		void DispatchComputeIndirect( const RHICommand::DispatchComputeIndirect& a_Data );
	};

}