#pragma once
#include "D3D12Common.h"
#include <Tridium/Graphics/RHI/RHICommandList.h>

namespace Tridium {

	class D3D12CommandList final : public RHICommandList
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		bool SetCommands( const RHICommandBuffer& a_CmdBuffer ) override;

		TODO( "Seperate cmd lists into graphics and compute" );
		ComPtr<D3D12::GraphicsCommandList> CommandList{};
		RHIShaderBindingLayout* CurrentSBL = nullptr;
		Array<ComPtr<D3D12::DescriptorHeap>> DescriptorHeaps{};
		uint32_t ShaderInputOffset = 0;

	private:
		void SetPipelineState( const RHICommand::SetPipelineState& a_Data );
		void SetShaderBindingLayout( const RHICommand::SetShaderBindingLayout& a_Data );
		void SetRenderTargets( const RHICommand::SetRenderTargets& a_Data );
		void ClearRenderTargets( const RHICommand::ClearRenderTargets& a_Data );
		void SetScissors( const RHICommand::SetScissors& a_Data );
		void SetViewports( const RHICommand::SetViewports& a_Data );
		void SetShaderInput( const RHICommand::SetShaderInput& a_Data );
		void SetIndexBuffer( const RHICommand::SetIndexBuffer& a_Data );
		void SetVertexBuffer( const RHICommand::SetVertexBuffer& a_Data );
		void SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology& a_Data );
		void Draw( const RHICommand::Draw& a_Data );
		void DrawIndexed( const RHICommand::DrawIndexed& a_Data );
		void ResourceBarrier( const RHICommand::ResourceBarrier& a_Data );
		void DispatchCompute( const RHICommand::DispatchCompute& a_Data );
		void FenceSignal( const RHICommand::FenceSignal& a_Data );
		void FenceWait( const RHICommand::FenceWait& a_Data );
		void Execute( const RHICommand::Execute& a_Data );
	};

}