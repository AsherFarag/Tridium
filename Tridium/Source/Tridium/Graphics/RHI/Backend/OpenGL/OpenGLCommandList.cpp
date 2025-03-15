#include "tripch.h"
#include "OpenGLCommandList.h"
#include "OpenGLTexture.h"
#include "OpenGLSampler.h"
#include "OpenGLMesh.h"
#include "OpenGLPipelineState.h"
#include "OpenGLState.h"

namespace Tridium {

    bool OpenGLCommandList::SetGraphicsCommands( const RHIGraphicsCommandBuffer& a_CmdBuffer )
    {
		// Execute the commands
		for ( const RHICommand& cmd : a_CmdBuffer.Commands )
		{
			switch ( cmd.Type() )
			{
			#define PerformCmd( _CmdType ) case ERHICommandType::_CmdType: _CmdType( cmd.Get<ERHICommandType::_CmdType>() ); break
				PerformCmd( SetShaderBindingLayout );
				PerformCmd( SetShaderInput );
				PerformCmd( ResourceBarrier );
				PerformCmd( SetGraphicsPipelineState );
				PerformCmd( SetRenderTargets );
				PerformCmd( ClearRenderTargets );
				PerformCmd( SetScissors );
				PerformCmd( SetViewports );
				PerformCmd( SetIndexBuffer );
				PerformCmd( SetVertexBuffer );
				PerformCmd( SetPrimitiveTopology );
				PerformCmd( Draw );
				PerformCmd( DrawIndexed );
				PerformCmd( SetComputePipelineState );
				PerformCmd( DispatchCompute );
				PerformCmd( DispatchComputeIndirect );
				PerformCmd( FenceSignal );
				PerformCmd( FenceWait );
				PerformCmd( Execute );
				default: ASSERT_LOG( false, "Invalid command type '{0}' being used in SetGraphicsCommands", RHI::GetCommandName( cmd.Type() ) ); break;
			}
		}

		return true;
    }

	bool OpenGLCommandList::SetComputeCommands( const RHIComputeCommandBuffer& a_CmdBuffer )
	{
		return false;
	}

	void OpenGLCommandList::SetShaderBindingLayout( const RHICommand::SetShaderBindingLayout& a_Data )
	{
		if ( a_Data.SBL == nullptr ) [[unlikely]]
		{
			ASSERT_LOG( false, "Attempting to bind a null shader binding layout!" );
			return;
		}

		OpenGLState::s_BoundSBL = SharedPtrCast<OpenGLShaderBindingLayout>( a_Data.SBL->shared_from_this() );
	}

	void OpenGLCommandList::SetShaderInput( const RHICommand::SetShaderInput& a_Data )
	{
		RHIShaderBindingLayoutRef sbl = OpenGLState::s_BoundSBL.lock();
		ASSERT_LOG( sbl, "No shader binding layout bound!" );

		const RHIShaderBinding& binding = sbl->GetDescriptor()->GetBindingFromName( a_Data.NameHash );
		TODO( "TEMP" );
		const GLint bindSlot = OpenGL2::GetUniformLocation( OpenGLState::s_BoundGraphicsPSO.lock()->GetShaderProgramID(), binding.Name );
		if ( bindSlot == -1 )
		{
			ASSERT_LOG( false, "Shader input not found in shader program: %s", binding.Name );
			return;
		}

		switch ( binding.BindingType )
		{
		case ERHIShaderBindingType::Constant:
		{
			if ( binding.IsInlined() )
			{
				// Inlined constant
				const float* asFloats = reinterpret_cast<const float*>( a_Data.Payload.InlineData );
				const double* asDoubles = reinterpret_cast<const double*>( a_Data.Payload.InlineData );
				const int32_t* asInt32s = reinterpret_cast<const int32_t*>( a_Data.Payload.InlineData );
				const uint32_t* asUint32s = reinterpret_cast<const uint32_t*>( a_Data.Payload.InlineData );

				const int32_t wordSize = binding.ConstantType.TensorType.GetSizeInBytes() / 4;
				const ERHIDataType dataType = binding.ConstantType.TensorType.ElementType;
				switch ( wordSize )
				{
				case 1:
					switch ( dataType )
					{
					case ERHIDataType::Float32: OpenGL3::Uniform1fv( bindSlot, 1, asFloats ); break;
					case ERHIDataType::Float64: OpenGL4::Uniform1dv( bindSlot, 1, asDoubles ); break;
					case ERHIDataType::Int32:   OpenGL2::Uniform1iv( bindSlot, 1, asInt32s ); break;
					case ERHIDataType::UInt32:  OpenGL3::Uniform1uiv( bindSlot, 1, asUint32s ); break;
					}
					break;

				case 2: // Vector2
					switch ( dataType )
					{
					case ERHIDataType::Float32: OpenGL2::Uniform2fv( bindSlot, 1, asFloats ); break;
					case ERHIDataType::Float64: OpenGL4::Uniform2dv( bindSlot, 1, asDoubles ); break;
					case ERHIDataType::Int32:   OpenGL2::Uniform2iv( bindSlot, 1, asInt32s ); break;
					case ERHIDataType::UInt32:  OpenGL3::Uniform2uiv( bindSlot, 1, asUint32s ); break;
					}
					break;

				case 3: // Vector3
					switch ( dataType )
					{
					case ERHIDataType::Float32: OpenGL3::Uniform3fv( bindSlot, 1, asFloats ); break;
					case ERHIDataType::Float64: OpenGL4::Uniform3dv( bindSlot, 1, asDoubles ); break;
					case ERHIDataType::Int32:   OpenGL2::Uniform3iv( bindSlot, 1, asInt32s ); break;
					case ERHIDataType::UInt32:  OpenGL3::Uniform3uiv( bindSlot, 1, asUint32s ); break;
					}
					break;

				case 4: // Vector4
					switch ( dataType )
					{
					case ERHIDataType::Float32: OpenGL3::Uniform4fv( bindSlot, 1, asFloats ); break;
					case ERHIDataType::Float64: OpenGL4::Uniform4dv( bindSlot, 1, asDoubles ); break;
					case ERHIDataType::Int32:   OpenGL2::Uniform4iv( bindSlot, 1, asInt32s ); break;
					case ERHIDataType::UInt32:  OpenGL3::Uniform4uiv( bindSlot, 1, asUint32s ); break;
					}
					break;

				case 9: // Matrix3
					switch ( dataType )
					{
					case ERHIDataType::Float32: OpenGL2::UniformMatrix3fv( bindSlot, 1, GL_FALSE, asFloats ); break;
					case ERHIDataType::Float64: OpenGL4::UniformMatrix3dv( bindSlot, 1, GL_FALSE, asDoubles ); break;
					}
					break;

				case 16: // Matrix4
					switch ( dataType )
					{
					case ERHIDataType::Float32: OpenGL2::UniformMatrix4fv( bindSlot, 1, GL_FALSE, asFloats ); break;
					case ERHIDataType::Float64: OpenGL4::UniformMatrix4dv( bindSlot, 1, GL_FALSE, asDoubles ); break;
					}
					break;
				}
			}
			else
			{
				// Referenced constant
				NOT_IMPLEMENTED;
			}
			break;
		}
		case ERHIShaderBindingType::Mutable:
			break;
		case ERHIShaderBindingType::Storage:
			break;
		case ERHIShaderBindingType::Texture:
		{
			if ( !a_Data.Payload.IsReference ) [[unlikely]]
			{
				ASSERT_LOG( false, "Invalid shader input - a Texture can not be inlined!" );
				break;
			}

			OpenGLTexture* tex = reinterpret_cast<OpenGLTexture*>( a_Data.Payload.References[0] );
			OpenGL4::BindTextureUnit( bindSlot, tex->GetGLHandle() );
			break;
		}
		case ERHIShaderBindingType::RWTexture:
			break;
		case ERHIShaderBindingType::Sampler:
		{
			if ( !a_Data.Payload.IsReference ) [[unlikely]]
			{
				ASSERT_LOG( false, "Invalid shader input - a dynamically-bound Sampler can not be inlined!" );
				break;
			}

			OpenGLSampler* sampler = reinterpret_cast<OpenGLSampler*>( a_Data.Payload.References[0] );
			OpenGL4::BindSampler( bindSlot, sampler->GetGLHandle() );

			break;
		}
		default:
			ASSERT_LOG( false, "Unknown shader binding type!" );
			return;
		}
	}

	void OpenGLCommandList::ResourceBarrier( const RHICommand::ResourceBarrier& a_Data )
	{
	}

	void OpenGLCommandList::SetGraphicsPipelineState( const RHICommand::SetGraphicsPipelineState& a_Data )
	{
		if ( a_Data.PSO == nullptr )
		{
			OpenGLState::s_BoundGraphicsPSO.reset();
			return;
		}

		OpenGLGraphicsPipelineState* pso = a_Data.PSO->As<OpenGLGraphicsPipelineState>();
		if ( !OpenGLState::s_BoundGraphicsPSO.expired() && OpenGLState::s_BoundGraphicsPSO.lock().get() == pso )
		{
			return;
		}

		OpenGLState::s_BoundGraphicsPSO = SharedPtrCast<OpenGLGraphicsPipelineState>( a_Data.PSO->shared_from_this() );

		// Bind the shader program
		OpenGL4::UseProgram( pso->GetShaderProgramID() );

		// Bind the VAO
		//if ( OpenGLState::s_BoundVAO != pso->GetVAO() )
		//{
		//	OpenGLState::s_BoundVAO = pso->GetVAO();
		//	OpenGL3::BindVertexArray( pso->GetVAO() );
		//}

		// Set the rasterizer state
	}

	void OpenGLCommandList::SetRenderTargets( const RHICommand::SetRenderTargets& a_Data )
	{
		if ( a_Data.RTV.Size() > 0 )
		{
			OpenGLTexture* rtv = a_Data.RTV[0]->As<OpenGLTexture>();
			OpenGL4::BindFramebuffer( GL_FRAMEBUFFER, rtv->GetGLHandle() );
		}
		else
		{
			OpenGL4::BindFramebuffer( GL_FRAMEBUFFER, 0 );
		}
	}

	void OpenGLCommandList::ClearRenderTargets( const RHICommand::ClearRenderTargets& a_Data )
	{

	}

	void OpenGLCommandList::SetScissors( const RHICommand::SetScissors& a_Data )
	{
		for ( uint32_t i = 0; i < a_Data.Rects.Size(); ++i )
		{
			const ScissorRect& scissor = a_Data.Rects[i];
			OpenGL4::ScissorIndexed( i, scissor.Left, scissor.Top, scissor.Right, scissor.Bottom );
		}
	}

	void OpenGLCommandList::SetViewports( const RHICommand::SetViewports& a_Data )
	{
		for ( uint32_t i = 0; i < a_Data.Viewports.Size(); ++i )
		{
			const Viewport& vp = a_Data.Viewports[i];
			OpenGL4::ViewportIndexedf( i, vp.X, vp.Y, vp.Width, vp.Height );
		}
	}

	void OpenGLCommandList::SetIndexBuffer( const RHICommand::SetIndexBuffer& a_Data )
	{
	}

	void OpenGLCommandList::SetVertexBuffer( const RHICommand::SetVertexBuffer& a_Data )
	{
		if ( a_Data.VBO == nullptr )
		{
			OpenGLState::s_BoundVBO = 0;
			return;
		}

		OpenGLVertexBuffer* vbo = a_Data.VBO->As<OpenGLVertexBuffer>();
		if ( OpenGLState::s_BoundVBO != vbo->GetGLHandle() )
		{
			OpenGLState::s_BoundVBO = vbo->GetGLHandle();
			OpenGL3::BindVertexArray( OpenGLState::s_BoundVAO );
			OpenGL1::BindBuffer( GL_ARRAY_BUFFER, vbo->GetGLHandle() );
			OpenGLState::s_BoundGraphicsPSO.lock()->ApplyVertexLayoutToVAO( OpenGLState::s_BoundVAO );
		}
	}

	void OpenGLCommandList::SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology& a_Data )
	{
		OpenGLState::s_BoundPrimitiveTopology = ToOpenGL::GetTopology( a_Data.Topology );
	}

	void OpenGLCommandList::Draw( const RHICommand::Draw& a_Data )
	{
		OpenGL1::DrawArrays( OpenGLState::s_BoundPrimitiveTopology, a_Data.VertexStart, a_Data.VertexCount );
	}

	void OpenGLCommandList::DrawIndexed( const RHICommand::DrawIndexed& a_Data )
	{
		TODO( "Fix index start i fink" );
		OpenGL2::DrawElements( OpenGLState::s_BoundPrimitiveTopology, a_Data.IndexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>( a_Data.IndexStart ) );
	}

	void OpenGLCommandList::SetComputePipelineState( const RHICommand::SetComputePipelineState& a_Data )
	{
	}

	void OpenGLCommandList::DispatchCompute( const RHICommand::DispatchCompute& a_Data )
	{
	}

	void OpenGLCommandList::DispatchComputeIndirect( const RHICommand::DispatchComputeIndirect& a_Data )
	{
	}

	void OpenGLCommandList::FenceSignal( const RHICommand::FenceSignal& a_Data )
	{
	}

	void OpenGLCommandList::FenceWait( const RHICommand::FenceWait& a_Data )
	{
	}

	void OpenGLCommandList::Execute( const RHICommand::Execute& a_Data )
	{
	}

} // namespace Tridium
