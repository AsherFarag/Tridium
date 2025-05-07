#include "tripch.h"
#include "OpenGLCommandList.h"
#include "OpenGLTexture.h"
#include "OpenGLBuffer.h"
#include "OpenGLSampler.h"
#include "OpenGLMesh.h"
#include "OpenGLPipelineState.h"
#include "OpenGLState.h"
#include "OpenGLShaderBindingLayout.h"

namespace Tridium {

	OpenGLCommandList::OpenGLCommandList( const RHICommandListDescriptor& a_Desc )
		: RHICommandList( a_Desc )
	{
	}

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
				default: ASSERT_LOG( false, "Invalid command type '{0}' being used in SetGraphicsCommands", RHI::GetCommandName( cmd.Type() ) ); break;
			}
		}

		OpenGL3::DeleteBuffers( m_UBOs.Size(), m_UBOs.Data() );
		m_UBOs.Clear();
		m_State = {};

		TODO( "Temp fix?" );
		GLState::BindProgram( 0 );
		GLState::s_BoundGraphicsPSO.reset();
		GLState::s_BoundSBL.reset();

		return true;
    }

	bool OpenGLCommandList::SetComputeCommands( const RHIComputeCommandBuffer& a_CmdBuffer )
	{
		NOT_IMPLEMENTED;
		return false;
	}

	void OpenGLCommandList::SetShaderBindingLayout( const RHICommand::SetShaderBindingLayout& a_Data )
	{
		if ( a_Data.SBL == nullptr ) [[unlikely]]
		{
			ASSERT_LOG( false, "Attempting to bind a null shader binding layout!" );
			return;
		}

		GLState::s_BoundSBL = SharedPtrCast<OpenGLShaderBindingLayout>( a_Data.SBL->shared_from_this() );
	}

	void OpenGLCommandList::SetShaderInput( const RHICommand::SetShaderInput& a_Data )
	{
		SharedPtr<OpenGLShaderBindingLayout> sbl = GLState::s_BoundSBL.lock();
		if ( !ASSERT_LOG( sbl, "No shader binding layout bound!" ) )
			return;

		SharedPtr<OpenGLGraphicsPipelineState> gpso = GLState::s_BoundGraphicsPSO.lock();
		if ( !ASSERT_LOG( gpso, "No graphics pipeline state bound!" ) )
			return;

		const RHIShaderBinding& binding = sbl->Descriptor().GetBindingFromName( a_Data.NameHash );
		const GLint uniformLocation = gpso->TryGetUniformLocation( binding.Name );
		if ( uniformLocation < 0 )
		{
			LOG( LogCategory::RHI, Error, "Uniform '{0}' not found in shader while setting shader input", binding.Name.String() );
			return;
		}

		switch ( binding.BindingType )
		{
		case ERHIShaderBindingType::Constant:
		{
			if ( binding.IsInlined() )
			{
				size_t offset = 0;
				int32_t bindingIndex = -1;
				for ( const auto& [name, tensorType] : binding.InlinedConstant->Tensors )
				{
					bindingIndex++;

					constexpr int32_t WordSize = 4;
					const int32_t numWords = tensorType.GetSizeInBytes() >> 2;
					const ERHIDataType dataType = tensorType.ElementType;
					TODO( "Get the uniform properly" );
					const GLint bindSlot = OpenGL3::GetUniformLocation( GLState::s_BoundProgram, name.c_str() );

					const float* asFloats = ReinterpretCast<const float*>( &a_Data.Payload.InlineData[offset * WordSize] );
					const double* asDoubles = ReinterpretCast<const double*>( &a_Data.Payload.InlineData[offset * WordSize] );
					const int32_t* asInt32s = ReinterpretCast<const int32_t*>( &a_Data.Payload.InlineData[offset * WordSize] );
					const uint32_t* asUint32s = ReinterpretCast<const uint32_t*>( &a_Data.Payload.InlineData[offset * WordSize] );

					// Increment the offset by the size of the tensor
					offset += numWords;

					switch ( numWords )
					{
					case 1: // Scalar
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
					default:
						ASSERT_LOG( false, "Invalid number of words in constant buffer!" );
						break;
					}
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
			NOT_IMPLEMENTED;
			break;
		case ERHIShaderBindingType::Storage:
			NOT_IMPLEMENTED;
			break;
		case ERHIShaderBindingType::Texture:
		{
			ASSERT_LOG( false, "OpenGL requires textures and samplers to be combined in the shader! - Use a Texture binding instead and set the sampler in the texture." );
			break;
		}
		case ERHIShaderBindingType::RWTexture:
			NOT_IMPLEMENTED;
			break;
		case ERHIShaderBindingType::Sampler:
		{
			ASSERT_LOG( false, "OpenGL requires textures and samplers to be combined in the shader! - Use a Texture binding instead and set the sampler in the texture." );
			break;
		}
		case ERHIShaderBindingType::CombinedSampler:
		{
			if ( !a_Data.Payload.IsReference ) [[unlikely]]
			{
				ASSERT_LOG( false, "Invalid shader input - a Texture can not be inlined!" );
				break;
			}

			// The names of Combined Samplers in GLSL have been set to the Texture name ( from HLSL )
			// So we can just bind the texture and sampler together
			OpenGLTexture* texture = static_cast<OpenGLTexture*>( a_Data.Payload.References[0] );
			if ( !( texture->Sampler ) )
			{
				ASSERT_LOG( false, "Texture has no sampler! - OpenGL requires Textures to have a Sampler, you can set the sampler on the RHITexture." );
				break;
			}

			OpenGLSampler* sampler = texture->Sampler->As<OpenGLSampler>();
			OpenGL4::BindTextureUnit( uniformLocation, texture->TextureObj );
			OpenGL4::BindSampler( uniformLocation, sampler->GetGLHandle() );
			break;
		}
		default:
			ASSERT_LOG( false, "Unknown shader binding type!" );
			return;
		}
	}

	void OpenGLCommandList::ResourceBarrier( const RHICommand::ResourceBarrier& a_Data )
	{
		//NOT_IMPLEMENTED;
	}

	void OpenGLCommandList::SetGraphicsPipelineState( const RHICommand::SetGraphicsPipelineState& a_Data )
	{
		if ( a_Data.PSO == nullptr )
		{
			GLState::s_BoundGraphicsPSO.reset();
			return;
		}

		OpenGLGraphicsPipelineState* pso = a_Data.PSO->As<OpenGLGraphicsPipelineState>();
		if ( !GLState::s_BoundGraphicsPSO.expired() && GLState::s_BoundGraphicsPSO.lock().get() == pso )
		{
			return;
		}

		GLState::s_BoundGraphicsPSO = SharedPtrCast<OpenGLGraphicsPipelineState>( a_Data.PSO->shared_from_this() );

		// Bind the shader program
		GLState::BindProgram( pso->GetShaderProgramID() );

		// Bind the VAO
		GLState::BindVertexArray( pso->GetVAO() );


		// Set blend state
		for ( uint32_t i = 0; i < RHIConstants::MaxColorTargets; ++i )
		{
			const RHIBlendState& blendState = a_Data.PSO->Descriptor().BlendState;
			if ( blendState.IsEnabled )
			{
				OpenGL3::Enablei( GL_BLEND, i );
				OpenGL4::BlendFuncSeparatei( i, 
					OpenGL::Translate( blendState.SrcFactorColor ),
					OpenGL::Translate( blendState.DstFactorColor ),
					OpenGL::Translate( blendState.SrcFactorAlpha ),
					OpenGL::Translate( blendState.DstFactorAlpha ) );
				OpenGL4::BlendEquationi( i, OpenGL::Translate( blendState.BlendEquation ) );
			}
			else
			{
				OpenGL3::Disablei( GL_BLEND, i );
			}
		}

		// Set depth state
		const RHIDepthState& depthState = a_Data.PSO->Descriptor().DepthState;
		if ( depthState.IsEnabled )
		{
			OpenGL3::Enable( GL_DEPTH_TEST );
			OpenGL3::DepthMask( depthState.DepthOp == ERHIDepthOp::Replace ? GL_TRUE : GL_FALSE );
			OpenGL3::DepthFunc( OpenGL::Translate( depthState.Comparison ) );
		}
		else
		{
			OpenGL3::Disable( GL_DEPTH_TEST );
		}

		// Set stencil state
		const RHIStencilState& stencilState = a_Data.PSO->Descriptor().StencilState;
		if ( stencilState.IsEnabled )
		{
			OpenGL3::Enable( GL_STENCIL_TEST );
			TODO( "Set the stencil state" );
			//OpenGL3::StencilFuncSeparate( GL_FRONT, OpenGL::Translate( stencilState.Comparison ), stencilState, stencilState.FrontFace.Mask );
			//OpenGL3::StencilOpSeparate( GL_FRONT, OpenGL::Translate( stencilState.Fail ), OpenGL::Translate( stencilState.FrontFace.DepthFailOp ), OpenGL::Translate( stencilState.FrontFace.PassOp ) );
			//OpenGL3::StencilFuncSeparate( GL_BACK, OpenGL::Translate( stencilState.Comparison ), stencilState.Reference, stencilState.BackFace.Mask );
			//OpenGL3::StencilOpSeparate( GL_BACK, OpenGL::Translate( stencilState.Fail ), OpenGL::Translate( stencilState.BackFace.DepthFailOp ), OpenGL::Translate( stencilState.BackFace.PassOp ) );
		}
		else
		{
			OpenGL3::Disable( GL_STENCIL_TEST );
		}

		// Set rasterizer state
		const RHIRasterizerState& rasterizerState = a_Data.PSO->Descriptor().RasterizerState;
		switch ( rasterizerState.CullMode )
		{
		case ERHIRasterizerCullMode::None:
			OpenGL3::Disable( GL_CULL_FACE );
			break;
		case ERHIRasterizerCullMode::Front:
			OpenGL3::Enable( GL_CULL_FACE );
			OpenGL3::CullFace( GL_FRONT );
			break;
		case ERHIRasterizerCullMode::Back:
			OpenGL3::Enable( GL_CULL_FACE );
			OpenGL3::CullFace( GL_BACK );
			break;
		default: ASSERT_LOG( false, "Invalid cull mode in Graphics Pipeline State!" ); break;
		}
		switch ( rasterizerState.FillMode )
		{
		case ERHIRasterizerFillMode::Point:
			OpenGL3::PolygonMode( GL_FRONT_AND_BACK, GL_POINT );
			break;
		case ERHIRasterizerFillMode::Solid:
			OpenGL3::PolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			break;
		case ERHIRasterizerFillMode::Wireframe:
			OpenGL3::PolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			break;
		default: ASSERT_LOG( false, "Invalid fill mode in Graphics Pipeline State!" ); break;
		}
		OpenGL3::FrontFace( rasterizerState.Clockwise ? GL_CW : GL_CCW );
	}

	void OpenGLCommandList::SetRenderTargets( const RHICommand::SetRenderTargets& a_Data )
	{
		if ( !a_Data.RTV[0] )
		{
			GLState::BindFBO( 0 );
			return;
		}

		if ( !GLState::s_FBO )
		{
			OpenGL3::GenFramebuffers( 1, &GLState::s_FBO );
		}

		GLState::BindFBO( GLState::s_FBO );

		const GLchar* label = "Asher's FBO";
		OpenGL4::ObjectLabel( GL_FRAMEBUFFER, GLState::s_FBO, strlen( label ), label );

		TODO( "Do we want to support this?" );
		constexpr int mipmapLevelToRenderTo = 0;

		// Bind the render targets
		m_State.NumColorTargets = a_Data.RTV.Size();
		for ( size_t i = 0; i < m_State.NumColorTargets; ++i )
		{
			OpenGLTexture* rtv = a_Data.RTV[i]->As<OpenGLTexture>();
			OpenGL3::FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, rtv->TextureObj, mipmapLevelToRenderTo );
		}

		// Bind the depth stencil target
		if ( a_Data.DSV )
		{
			OpenGLTexture* dsv = a_Data.DSV->As<OpenGLTexture>();
			const bool hasStencil = GetRHIFormatInfo( dsv->Descriptor().Format ).HasStencil;
			OpenGL3::FramebufferTexture2D( GL_FRAMEBUFFER, hasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dsv->TextureObj, mipmapLevelToRenderTo );
		}

		bool isComplete = OpenGL3::CheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE;
		if ( !isComplete )
		{
			// Get the error msg
			GLenum status = OpenGL3::CheckFramebufferStatus( GL_FRAMEBUFFER );
			switch ( status )
			{
			case GL_FRAMEBUFFER_UNDEFINED: LOG( LogCategory::RHI, Error, "Framebuffer is undefined!" ); break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: LOG( LogCategory::RHI, Error, "Framebuffer incomplete attachment!" ); break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: LOG( LogCategory::RHI, Error, "Framebuffer incomplete missing attachment!" ); break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: LOG( LogCategory::RHI, Error, "Framebuffer incomplete draw buffer!" ); break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: LOG( LogCategory::RHI, Error, "Framebuffer incomplete read buffer!" ); break;
			case GL_FRAMEBUFFER_UNSUPPORTED: LOG( LogCategory::RHI, Error, "Framebuffer unsupported!" ); break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: LOG( LogCategory::RHI, Error, "Framebuffer incomplete multisample!" ); break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: LOG( LogCategory::RHI, Error, "Framebuffer incomplete layer targets!" ); break;
			default: LOG( LogCategory::RHI, Error, "Framebuffer unknown error!" ); break;
			}

			ASSERT_LOG( false, "Framebuffer is not complete!" );
			return;
		}

		// Set the draw buffers
		constexpr GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
		OpenGL3::DrawBuffers( a_Data.RTV.Size(), drawBuffers );
	}

	void OpenGLCommandList::ClearRenderTargets( const RHICommand::ClearRenderTargets& a_Data )
	{
		if ( a_Data.ClearFlags.HasFlag( ERHIClearFlags::Color ) )
		{
			for ( size_t i = 0; i < m_State.NumColorTargets; ++i )
			{
				const Color& color = a_Data.ClearColorValues[i];
				OpenGL3::ClearBufferfv( GL_COLOR, i, &color.r );
			}
		}
		if ( a_Data.ClearFlags.HasFlag( ERHIClearFlags::Depth ) )
		{
			OpenGL3::ClearBufferfv( GL_DEPTH, 0, &a_Data.DepthValue );
		}
		if ( a_Data.ClearFlags.HasFlag( ERHIClearFlags::Stencil ) )
		{
			GLint stencil = a_Data.StencilValue;
			OpenGL3::ClearBufferiv( GL_STENCIL, 0, &stencil );
		}
	}

	void OpenGLCommandList::SetScissors( const RHICommand::SetScissors& a_Data )
	{
		for ( uint32_t i = 0; i < a_Data.Rects.Size(); ++i )
		{
			const RHIScissorRect& scissor = a_Data.Rects[i];
			const GLint left = scissor.Left;
			const GLint bottom = scissor.Bottom;
			const GLsizei width = scissor.Right - scissor.Left;
			const GLsizei height = scissor.Bottom - scissor.Top;
			OpenGL4::ScissorIndexed( i, left, bottom, width, height );
		}
	}

	void OpenGLCommandList::SetViewports( const RHICommand::SetViewports& a_Data )
	{
		//OpenGL1::Viewport( a_Data.Viewports[0].X, a_Data.Viewports[0].Y, a_Data.Viewports[0].Width, a_Data.Viewports[0].Height );
		for ( uint32_t i = 0; i < a_Data.Viewports.Size(); ++i )
		{
			const RHIViewport& vp = a_Data.Viewports[i];
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
			GLState::s_BoundVBO = 0;
		}
		else
		{
			GLState::s_BoundVBO = a_Data.VBO->As<OpenGLBuffer>()->BufferObj;
		}

		if ( GLState::s_BoundVBO && GLState::s_BoundIBO )
		{
			// Bind the VAO
			const GLuint vao = GLState::GetOrEmplaceCachedVAO( GLState::s_BoundVBO, GLState::s_BoundIBO );
			GLState::BindVertexArray( vao );
			OpenGL1::BindBuffer( GL_ARRAY_BUFFER, GLState::s_BoundVBO );
			OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, GLState::s_BoundIBO );

			GLState::s_BoundGraphicsPSO.lock()->ApplyVertexLayoutToVAO( vao );
		}
		else if ( GLState::s_BoundVBO )
		{
			// Bind the VAO
			const GLuint vao = GLState::GetOrEmplaceCachedVAO( GLState::s_BoundVBO );
			GLState::BindVertexArray( vao );
			OpenGL1::BindBuffer( GL_ARRAY_BUFFER, GLState::s_BoundVBO );

			GLState::s_BoundGraphicsPSO.lock()->ApplyVertexLayoutToVAO( vao );
		}
		else
		{
			GLState::BindVertexArray( 0 );
		}
	}

	void OpenGLCommandList::SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology& a_Data )
	{
		GLState::s_BoundPrimitiveTopology = ToOpenGL::GetTopology( a_Data.Topology );
	}

	void OpenGLCommandList::Draw( const RHICommand::Draw& a_Data )
	{
		OpenGL1::DrawArrays( GLState::s_BoundPrimitiveTopology, a_Data.VertexStart, a_Data.VertexCount );
	}

	void OpenGLCommandList::DrawIndexed( const RHICommand::DrawIndexed& a_Data )
	{
		TODO( "Fix index start i fink" );
		OpenGL2::DrawElements( GLState::s_BoundPrimitiveTopology, a_Data.IndexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>( a_Data.IndexStart ) );
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

} // namespace Tridium
