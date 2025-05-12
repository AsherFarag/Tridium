#pragma once
#include "OpenGLCommon.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHICommandList_OpenGLImpl, RHICommandList )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHICommandList_OpenGLImpl, ERHInterfaceType::OpenGL );

		RHICommandList_OpenGLImpl( const DescriptorType& a_Desc );
		virtual bool Release() override { return true; }
		virtual bool IsValid() const override { return true; }
		virtual const void* NativePtr() const override { return nullptr; }

		virtual bool SetGraphicsCommands( const RHIGraphicsCommandBuffer& a_CmdBuffer ) override;
		virtual bool SetComputeCommands( const RHIComputeCommandBuffer& a_CmdBuffer ) override;
		virtual bool IsCompleted() const override { return true; }
		virtual void WaitUntilCompleted() override {}

	private:
		Array<GLuint> m_UBOs;
		struct State
		{
			uint32_t NumColorTargets = 0;
		} m_State;

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
	};

} // namespace Tridium