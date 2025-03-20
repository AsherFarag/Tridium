#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	class OpenGLCommandList : public RHICommandList
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );

		bool Commit( const void* a_Params ) override { ParamsToDescriptor<RHICommandListDescriptor>( a_Params ); return true; }
		bool Release() override { return true; }
		bool IsValid() const override { return true; }
		const void* NativePtr() const override { return nullptr; }

		bool SetGraphicsCommands( const RHIGraphicsCommandBuffer& a_CmdBuffer ) override;
		bool SetComputeCommands( const RHIComputeCommandBuffer& a_CmdBuffer ) override;

	private:
		Array<GLuint> m_UBOs;

		void SetShaderBindingLayout( const RHICommand::SetShaderBindingLayout& a_Data );
		void SetShaderInput( const RHICommand::SetShaderInput& a_Data );
		void ResourceBarrier( const RHICommand::ResourceBarrier& a_Data );

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

		void FenceSignal( const RHICommand::FenceSignal& a_Data );
		void FenceWait( const RHICommand::FenceWait& a_Data );
		void Execute( const RHICommand::Execute& a_Data );
	};

} // namespace Tridium