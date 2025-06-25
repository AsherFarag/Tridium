#include "tripch.h"
#include "RHI_OpenGLImpl.h"

#undef RHI_DEBUG_CMD_PARAM
#define RHI_DEBUG_SRC_LOC_PARAM const SourceLocation& RHI_DEBUG_SRC_LOC

namespace Tridium::OpenGL {

	RHICommandList_OpenGLImpl::RHICommandList_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHICommandList( a_Device, a_Desc )
	{
		m_InlinedConstantsUBO.Create();
		OpenGL1::BindBuffer( GL_UNIFORM_BUFFER, m_InlinedConstantsUBO );
		OpenGL1::BufferData( GL_UNIFORM_BUFFER, RHIConstants::MaxInlinedConstantsSize, nullptr, GL_DYNAMIC_DRAW );
		OpenGL1::BindBuffer( GL_UNIFORM_BUFFER, 0 );
	}

	bool RHICommandList_OpenGLImpl::Release()
	{
		m_InlinedConstantsUBO.Release();
		m_Deferred.CommandBuffer.Clear();
		return true;
	}

	bool RHICommandList_OpenGLImpl::Open()
	{
		if ( m_IsOpen )
		{
			RHI_DEV_CHECK( m_IsOpen, "Command list is already open!" );
			return false;
		}

		// Clear
		m_Deferred.CommandBuffer.Clear();
		m_CurrentGraphicsState = {};
		m_GraphicsStateValid = false;

		m_IsOpen = true;
		return true;
	}

	bool RHICommandList_OpenGLImpl::Close()
	{
		RHI_DEBUG_OP( m_DebugCommands.Clear() );

		if ( !m_IsOpen )
		{
			RHI_DEV_CHECK( m_IsOpen, "Command list is not open!" );
			return false;
		}

		m_IsOpen = false;
		return true;
	}

	void RHICommandList_OpenGLImpl::ClearState()
	{
		m_CurrentGraphicsState = {};
		m_GraphicsStateValid = false;
	}

	void RHICommandList_OpenGLImpl::ResourceBarriers( Span<const RHIResourceBarrier> a_Barriers, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::ResourceBarriers( a_Barriers, RHI_DEBUG_SRC_LOC );
	}

	void RHICommandList_OpenGLImpl::UpdateBuffer( IRHIBuffer& a_Buffer, const void* a_Data, size_t a_DataSizeBytes, size_t a_DstOffsetBytes, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::UpdateBuffer( a_Buffer, a_Data, a_DataSizeBytes, a_DstOffsetBytes, RHI_DEBUG_SRC_LOC );
		m_ReferencedObjects.EmplaceBack( a_Buffer.SharedFromThis() );

		if ( IsImmediate() )
		{
			UpdateBuffer_Impl( a_Buffer, a_Data, a_DataSizeBytes, a_DstOffsetBytes );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::UpdateBuffer{ 
				.Buffer = a_Buffer.SharedFromThis(), 
				.Data = a_Data, 
				.DataSizeBytes = a_DataSizeBytes, 
				.DstOffsetBytes = a_DstOffsetBytes 
			} );
		}
	}

	void RHICommandList_OpenGLImpl::CopyBuffer( IRHIBuffer& a_DstBuffer, size_t a_DstOffsetBytes, IRHIBuffer& a_SrcBuffer, RHIBufferRange a_SrcRange, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::CopyBuffer( a_DstBuffer, a_DstOffsetBytes, a_SrcBuffer, a_SrcRange, RHI_DEBUG_SRC_LOC );

		m_ReferencedObjects.EmplaceBack( a_DstBuffer.SharedFromThis() );
		m_ReferencedObjects.EmplaceBack( a_SrcBuffer.SharedFromThis() );

		if ( IsImmediate() )
		{
			CopyBuffer_Impl( a_DstBuffer, a_DstOffsetBytes, a_SrcBuffer, a_SrcRange );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::CopyBuffer{ 
				.DstBuffer = a_DstBuffer.SharedFromThis(), 
				.DstOffsetBytes = a_DstOffsetBytes, 
				.SrcBuffer = a_SrcBuffer.SharedFromThis(), 
				.SrcRange = a_SrcRange 
			} );
		}
	}

	void RHICommandList_OpenGLImpl::UpdateTexture( IRHITexture& a_Texture, const RHITextureSlice& a_DstSlice, RHITextureSubresourceData a_Data, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::UpdateTexture( a_Texture, a_DstSlice, a_Data, RHI_DEBUG_SRC_LOC );

		m_ReferencedObjects.EmplaceBack( a_Texture.SharedFromThis() );

		if ( IsImmediate() )
		{
			UpdateTexture_Impl( a_Texture, a_DstSlice, a_Data );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::UpdateTexture{ 
				.Texture = a_Texture.SharedFromThis(), 
				.DstSlice = a_DstSlice, 
				.Data = a_Data 
			} );
		}
	}

	void RHICommandList_OpenGLImpl::CopyTexture( IRHITexture& a_DstTexture, const RHITextureSlice& a_DstSlice, IRHITexture& a_SrcTexture, const RHITextureSlice& a_SrcSlice, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::CopyTexture( a_DstTexture, a_DstSlice, a_SrcTexture, a_SrcSlice, RHI_DEBUG_SRC_LOC );

		m_ReferencedObjects.EmplaceBack( a_DstTexture.SharedFromThis() );
		m_ReferencedObjects.EmplaceBack( a_SrcTexture.SharedFromThis() );

		if ( IsImmediate() )
		{
			CopyTexture_Impl( a_DstTexture, a_DstSlice, a_SrcTexture, a_SrcSlice );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::CopyTexture{ 
				.DstTexture = a_DstTexture.SharedFromThis(), 
				.DstSlice = a_DstSlice, 
				.SrcTexture = a_SrcTexture.SharedFromThis(), 
				.SrcSlice = a_SrcSlice 
			} );
		}
	}

	void RHICommandList_OpenGLImpl::SetInlinedConstants( const void* a_Data, uint32_t a_SizeBytes, uint32_t a_DstOffsetBytes, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
		IRHICommandList::SetInlinedConstants( a_Data, a_SizeBytes, a_DstOffsetBytes, RHI_DEBUG_SRC_LOC );

		if ( IsImmediate() )
		{
			SetInlinedConstants_Impl( a_Data, a_SizeBytes, a_DstOffsetBytes );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::SetInlinedConstants{ 
				.Data = Span<const uint8_t>( Cast<const uint8_t*>( a_Data ), a_SizeBytes ), 
				.DstOffsetBytes = a_DstOffsetBytes,
				.SizeBytes = a_SizeBytes
			} );
		}
	}

	void RHICommandList_OpenGLImpl::SetGraphicsState( const RHIGraphicsState& a_GraphicsState, RHI_DEBUG_SRC_LOC_PARAM ) 
	{
		IRHICommandList::SetGraphicsState( a_GraphicsState, RHI_DEBUG_SRC_LOC );

		m_ReferencedObjects.EmplaceBack( a_GraphicsState.PipelineState->Shared() );

		for ( const auto& attachment : a_GraphicsState.Framebuffer.ColorAttachments ) 
		{
			if ( attachment.Texture )
				m_ReferencedObjects.EmplaceBack( attachment.Texture->Shared() );
		}
		if ( a_GraphicsState.Framebuffer.DepthStencilAttachment.Texture )
			m_ReferencedObjects.EmplaceBack( a_GraphicsState.Framebuffer.DepthStencilAttachment.Texture->Shared() );

		if ( a_GraphicsState.VertexBuffer )
			m_ReferencedObjects.EmplaceBack( a_GraphicsState.VertexBuffer->Shared() );
		if ( a_GraphicsState.IndexBuffer )
			m_ReferencedObjects.EmplaceBack( a_GraphicsState.IndexBuffer->Shared() );

		for ( const auto& bindingSet : a_GraphicsState.BindingSets )
		{
			if ( bindingSet )
				m_ReferencedObjects.EmplaceBack( bindingSet->Shared() );
		}

		if ( IsImmediate() )
		{
			SetGraphicsState_Impl( a_GraphicsState );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::SetGraphicsState{ 
				.GraphicsState = a_GraphicsState
				} );
		}
	}

	void RHICommandList_OpenGLImpl::ClearRenderTargets( ERHIClearFlags a_Flags, Color a_ClearColor, float a_DepthValue, uint8_t a_StencilValue, int32_t a_ColorAttachmentIndex, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::ClearRenderTargets( a_Flags, a_ClearColor, a_DepthValue, a_StencilValue, a_ColorAttachmentIndex, RHI_DEBUG_SRC_LOC );
		if ( IsImmediate() )
		{
			ClearRenderTargets_Impl( a_Flags, a_ClearColor, a_DepthValue, a_StencilValue, a_ColorAttachmentIndex );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::ClearRenderTargets{ 
				.Flags = a_Flags, 
				.ClearColor = a_ClearColor, 
				.DepthValue = a_DepthValue, 
				.StencilValue = a_StencilValue, 
				.ColorAttachmentIndex = a_ColorAttachmentIndex 
			} );
		}
	}

	void RHICommandList_OpenGLImpl::SetViewportState( const RHIViewportState& a_Viewports, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::SetViewportState( a_Viewports, RHI_DEBUG_SRC_LOC );
		if ( IsImmediate() )
		{
			SetViewportState_Impl( a_Viewports );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::SetViewportState{ 
				.Viewports = a_Viewports 
			} );
		}
	}

	void RHICommandList_OpenGLImpl::Draw( const RHIDrawArgs& a_DrawArgs, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::Draw( a_DrawArgs, RHI_DEBUG_SRC_LOC );
		if ( IsImmediate() )
		{
			Draw_Impl( a_DrawArgs );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::Draw{ 
				.DrawArgs = a_DrawArgs 
			} );
		}
	}
	
	void RHICommandList_OpenGLImpl::PushDebugGroup( StringView a_Name )
	{
		if ( IsImmediate() )
		{
			PushDebugGroup_Impl( a_Name );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::PushDebugGroup{ 
				.Name = a_Name 
			} );
		}
	}

	void RHICommandList_OpenGLImpl::PopDebugGroup()
	{
		if ( IsImmediate() )
		{
			PopDebugGroup_Impl();
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::PopDebugGroup{} );
		}
	}

	void RHICommandList_OpenGLImpl::InsertDebugMarker( StringView a_Name )
	{
		if ( IsImmediate() )
		{
			InsertDebugMarker_Impl( a_Name );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::InsertDebugMarker{ 
				.Name = a_Name 
			} );
		}
	}

	void RHICommandList_OpenGLImpl::Flush()
	{
		if ( !IsImmediate() )
			FlushCommandBuffer();

		OpenGL1::Flush();

		m_ReferencedObjects.Clear();
	}

	void RHICommandList_OpenGLImpl::BindGraphicsPipelineState( const RHIGraphicsPipelineState_OpenGLImpl& a_GraphicsPipelineState )
	{
		const RHIGraphicsPipelineStateDesc& desc = a_GraphicsPipelineState.Desc();

		// Topology
		// We don't need to set the topology explicitly in OpenGL, as it is determined by the draw call.

		// Blend State
		{
			const RHIBlendState& blendState = desc.BlendState;
			OpenGL1::Enable( GL_BLEND );

			if ( blendState.AlphaToCoverageEnabled )
				OpenGL1::Enable( GL_SAMPLE_ALPHA_TO_COVERAGE );
			else
				OpenGL1::Disable( GL_SAMPLE_ALPHA_TO_COVERAGE );

			if ( blendState.IndependentBlendEnabled )
			{
				for ( uint32_t i = 0; i < blendState.RenderTargets.MaxSize(); ++i )
				{
					const auto& rtBlendState = blendState.RenderTargets[i];
					if ( rtBlendState.BlendEnabled )
					{
						OpenGL3::Enablei( GL_BLEND, i );
						OpenGL4::BlendFuncSeparatei( i,
							Translate( rtBlendState.SrcColor ),
							Translate( rtBlendState.DstColor ),
							Translate( rtBlendState.SrcAlpha ),
							Translate( rtBlendState.DstAlpha ) );

						OpenGL4::BlendEquationSeparatei( i,
							Translate( rtBlendState.BlendOpColor ),
							Translate( rtBlendState.BlendOpAlpha ) );

						EnumFlags colorWriteMask = rtBlendState.ColorWriteMask;
						OpenGL3::ColorMaski( i,
							colorWriteMask.HasFlag( ERHIColorMask::Red ) ? GL_TRUE : GL_FALSE,
							colorWriteMask.HasFlag( ERHIColorMask::Green ) ? GL_TRUE : GL_FALSE,
							colorWriteMask.HasFlag( ERHIColorMask::Blue ) ? GL_TRUE : GL_FALSE,
							colorWriteMask.HasFlag( ERHIColorMask::Alpha ) ? GL_TRUE : GL_FALSE );
					}
					else
					{
						OpenGL3::Disablei( GL_BLEND, i );
					}
				}
			}
			else
			{
				const auto& rtBlendState = blendState.RenderTargets[0];
				OpenGL3::Enable( GL_BLEND );
				OpenGL4::BlendFuncSeparate(
					Translate( rtBlendState.SrcColor ),
					Translate( rtBlendState.DstColor ),
					Translate( rtBlendState.SrcAlpha ),
					Translate( rtBlendState.DstAlpha ) );

				OpenGL4::BlendEquationSeparate(
					Translate( rtBlendState.BlendOpColor ),
					Translate( rtBlendState.BlendOpAlpha ) );

				EnumFlags colorWriteMask = rtBlendState.ColorWriteMask;
				OpenGL1::ColorMask(
					colorWriteMask.HasFlag( ERHIColorMask::Red ) ? GL_TRUE : GL_FALSE,
					colorWriteMask.HasFlag( ERHIColorMask::Green ) ? GL_TRUE : GL_FALSE,
					colorWriteMask.HasFlag( ERHIColorMask::Blue ) ? GL_TRUE : GL_FALSE,
					colorWriteMask.HasFlag( ERHIColorMask::Alpha ) ? GL_TRUE : GL_FALSE );
			}

			// OpenGL only supports a single logic operation for blending, so we set it globally.
			if ( blendState.RenderTargets[0].LogicOpEnabled )
			{
				OpenGL1::Enable( GL_COLOR_LOGIC_OP );
				OpenGL1::LogicOp( Translate( blendState.RenderTargets[0].LogicOp ) );
			}
			else
			{
				OpenGL1::Disable( GL_COLOR_LOGIC_OP );
			}
		}

		// Depth State
		{
			const RHIDepthState& depthState = desc.DepthState;
			if ( depthState.DepthTestEnabled )
				OpenGL1::Enable( GL_DEPTH_TEST );
			else
				OpenGL1::Disable( GL_DEPTH_TEST );

			OpenGL1::DepthMask( depthState.DepthWriteEnabled ? GL_TRUE : GL_FALSE );
			OpenGL1::DepthFunc( Translate( depthState.Comparison ) );
		}

		// Stencil State
		{
			const RHIStencilState& stencilState = desc.StencilState;
			if ( stencilState.Enabled )
			{
				OpenGL3::Enable( GL_STENCIL_TEST );
				OpenGL3::StencilFuncSeparate( GL_FRONT, Translate( stencilState.FrontFace.Comparison ), stencilState.RefValue, stencilState.ReadMask );
				OpenGL3::StencilOpSeparate( GL_FRONT, Translate( stencilState.FrontFace.StencilFailOp ), Translate( stencilState.FrontFace.DepthFailOp ), Translate( stencilState.FrontFace.PassOp ) );
				OpenGL3::StencilFuncSeparate( GL_BACK, Translate( stencilState.BackFace.Comparison ), stencilState.RefValue, stencilState.ReadMask );
				OpenGL3::StencilOpSeparate( GL_BACK, Translate( stencilState.BackFace.StencilFailOp ), Translate( stencilState.BackFace.DepthFailOp ), Translate( stencilState.BackFace.PassOp ) );
			}
			else
			{
				OpenGL3::Disable( GL_STENCIL_TEST );
			}
		}

		// Rasterizer State
		{
			const RHIRasterizerState& rasterizerState = desc.RasterizerState;

			if ( rasterizerState.CullMode == ERHICullMode::None )
			{
				OpenGL1::Disable( GL_CULL_FACE );
			}
			else
			{
				OpenGL3::Enable( GL_CULL_FACE );
				OpenGL3::CullFace( rasterizerState.CullMode == ERHICullMode::Front ? GL_FRONT : GL_BACK );
			}

			OpenGL3::PolygonMode( GL_FRONT_AND_BACK, 
				  rasterizerState.FillMode == ERHIFillMode::Solid ? GL_FILL        // Solid mode
				: rasterizerState.FillMode == ERHIFillMode::Wireframe ? GL_LINE    // Wireframe mode
				: GL_POINT );                                                      // Point mode

			OpenGL3::FrontFace( rasterizerState.Clockwise ? GL_CW : GL_CCW );

			if ( rasterizerState.DepthClipEnabled )
				OpenGL1::Enable( GL_DEPTH_CLAMP );
			else
				OpenGL1::Disable( GL_DEPTH_CLAMP );

			if ( rasterizerState.ScissorEnabled )
				OpenGL1::Enable( GL_SCISSOR_TEST );
			else
				OpenGL1::Disable( GL_SCISSOR_TEST );

			if ( rasterizerState.AnitaliasedLinesEnabled )
				OpenGL1::Enable( GL_LINE_SMOOTH );
			else
				OpenGL1::Disable( GL_LINE_SMOOTH );

			if ( rasterizerState.DepthBias != 0 && rasterizerState.SlopeScaledDepthBias != 0.0f )
			{
				OpenGL3::Enable( GL_POLYGON_OFFSET_FILL );
				OpenGL3::PolygonOffset( rasterizerState.SlopeScaledDepthBias, Cast<float>( rasterizerState.DepthBias ) );
			}
			else
			{
				OpenGL3::Disable( GL_POLYGON_OFFSET_FILL );
			}
		}

		// Bind the shader program
		{
			OpenGL2::UseProgram( a_GraphicsPipelineState.GetShaderProgramID() );
		}
	}

	void RHICommandList_OpenGLImpl::BindFramebuffer( const RHIFramebuffer& a_Framebuffer )
	{
		m_FramebufferObj.Create();
		OpenGL3::BindFramebuffer( GL_FRAMEBUFFER, m_FramebufferObj );

		TODO( "Do we want to support this?" );
		constexpr int mipmapLevelToRenderTo = 0;

		// Bind color attachments
		for ( size_t i = 0; i < a_Framebuffer.ColorAttachments.Size(); ++i )
		{
			const auto& attachment = a_Framebuffer.ColorAttachments[i];
			if ( attachment.Texture )
			{
				const auto* texture = attachment.Texture->As<RHITexture_OpenGLImpl>();
				const GLenum target = attachment.ReadOnly ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER;
				OpenGL3::FramebufferTexture2D( target, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->TextureObj, mipmapLevelToRenderTo );
			}
		}

		if ( a_Framebuffer.DepthStencilAttachment )
		{
			const auto* depthStencilTexture = a_Framebuffer.DepthStencilAttachment.Texture->As<RHITexture_OpenGLImpl>();
			const GLenum target = a_Framebuffer.DepthStencilAttachment.ReadOnly ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER;
			const GLenum attachmentType = GetRHIFormatInfo( a_Framebuffer.DepthStencilAttachment.Texture->Desc().Format ).HasStencil
				? GL_DEPTH_STENCIL_ATTACHMENT
				: GL_DEPTH_ATTACHMENT;
			OpenGL3::FramebufferTexture2D( target, attachmentType, GL_TEXTURE_2D, depthStencilTexture->TextureObj, mipmapLevelToRenderTo );
		}

	#if RHI_DEBUG_ENABLED
		const GLenum status = OpenGL3::CheckFramebufferStatus( GL_FRAMEBUFFER );
		if ( status != GL_FRAMEBUFFER_COMPLETE )
		{
			StringView statusStr{};
			switch ( status )
			{
			case GL_FRAMEBUFFER_UNDEFINED: statusStr = "GL_FRAMEBUFFER_UNDEFINED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: statusStr = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: statusStr = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: statusStr = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: statusStr = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
			case GL_FRAMEBUFFER_UNSUPPORTED: statusStr = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: statusStr = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: statusStr = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
			}

			ASSERT( false, "Framebuffer is not complete - Status: {0} ({1})", status, statusStr );
		}
	#endif

		// Set the draw buffers
		constexpr GLenum drawBuffers[8] = { 
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 
		};

		if ( a_Framebuffer.ColorAttachments.Size() > 0 )
			OpenGL3::DrawBuffers( a_Framebuffer.ColorAttachments.Size(), drawBuffers );
		else
			OpenGL3::DrawBuffer( GL_NONE ); // No color attachments, disable draw buffers
	}

	void RHICommandList_OpenGLImpl::BindGraphicsBindings( const RHIGraphicsState& a_GraphicsState )
	{
		RHI_DEV_CHECK( a_GraphicsState.PipelineState, "Graphics state must have a valid pipeline state!" );

		const UniformLayout& uniformLayout = a_GraphicsState.PipelineState->As<RHIGraphicsPipelineState_OpenGLImpl>()->UniformLayout;
		for ( uint32_t bindingSetIndex = 0; bindingSetIndex < a_GraphicsState.BindingSets.Size(); ++bindingSetIndex )
		{
			const auto* bindingSet = Cast<const RHIBindingSet_OpenGLImpl*>( a_GraphicsState.BindingSets[bindingSetIndex] );
			if ( bindingSet == nullptr )
				continue;

			const IRHIBindingLayout* bindingLayout = bindingSet->Desc().Layout.get();
			const RHIBindingSetItemArray& bindings = bindingSet->Desc().Bindings;
			RHI_DEV_CHECK( bindingLayout, "Binding set {0} has no layout!", bindingSetIndex );

			for ( const RHIBindingSetItem& binding : bindings )
			{
				auto uniformIt = uniformLayout.Layouts[bindingSetIndex].find( binding.Slot );
				if ( uniformIt == uniformLayout.Layouts[bindingSetIndex].end() )
					continue;

				const Uniform& uniform = uniformIt->second;
				GLint bindingPoint = uniform.BindingPoint;

				if ( bindingPoint < 0 )
				{
					LOG( LogCategory::Debug, Warn, "Binding point for binding set {0}, slot {1} is invalid ({2})! Skipping binding.", 
						bindingSetIndex, binding.Slot, bindingPoint );
					continue; // Invalid binding point, skip this binding
				}

				bool isBufferBinding = false;
				bool isTextureBinding = false;
				GLenum bufferTarget = 0;

				switch ( binding.Type )
				{
				case ERHIBindingType::Unknown: break;
				case ERHIBindingType::InlinedConstants: break;
				case ERHIBindingType::ConstantBuffer: 
					isBufferBinding = true;
					bufferTarget = GL_UNIFORM_BUFFER;
					break;
				case ERHIBindingType::StructuredBuffer:
					isBufferBinding = true;
					bufferTarget = GL_SHADER_STORAGE_BUFFER;
					break;
				case ERHIBindingType::StorageBuffer:
					isBufferBinding = true;
					bufferTarget = GL_ARRAY_BUFFER;
					break;
				case ERHIBindingType::Texture:
				case ERHIBindingType::StorageTexture:
					isTextureBinding = true;
					break;

				#if RHI_DEBUG_ENABLED
				default:
					RHI_DEV_CHECK( false, "Unknown binding type {0} for binding set {1}, binding {2}!",
						ToString( binding.Type ), bindingSetIndex, binding.Slot );
					break;
				#endif // RHI_DEBUG_ENABLED
				}

				if ( isBufferBinding )
				{
					if ( auto* buffer = binding.Resource->As<RHIBuffer_OpenGLImpl>() )
					{
						if ( binding.Range.IsEntireBuffer() )
							OpenGL3::BindBufferBase( bufferTarget, bindingPoint, buffer->BufferObj );
						else
							OpenGL3::BindBufferRange( bufferTarget, bindingPoint, buffer->BufferObj, binding.Range.Offset, binding.Range.Size );
					}
					else
					{
						OpenGL3::BindBufferBase( bufferTarget, bindingPoint, 0 );
					}
				}
				else if ( isTextureBinding )
				{
					if ( auto* texture = binding.Resource->As<RHITexture_OpenGLImpl>() )
					{
						OpenGL4::BindTextureUnit( uniform.BindingPoint, texture->TextureObj );
						TODO( "Handle texture subresources better" );

						RHISampler sampler = texture->Desc().DefaultSampler;
						if ( binding.Sampler.Valid() )
							sampler = binding.Sampler.Unpack();

						const bool isDepth = GetRHIFormatInfo( binding.Format ).HasDepth;
						GLuint glSampler = Device()->ResourceCache().GetOrCreateSampler( sampler, isDepth );
						OpenGL4::BindSampler( uniform.BindingPoint, glSampler );
					}
					else
					{
						OpenGL4::BindTextureUnit( uniform.BindingPoint, 0 ); // Unbind texture
						OpenGL4::BindSampler( uniform.BindingPoint, 0 ); // Unbind sampler
					}
				}
			}
		}
	}

	//
	// === Command Implementations ===
	//

	void RHICommandList_OpenGLImpl::UpdateBuffer_Impl( IRHIBuffer& a_Buffer, const void* a_Data, size_t a_DataSizeBytes, size_t a_DstOffsetBytes )
	{
		auto* buffer = a_Buffer.As<RHIBuffer_OpenGLImpl>();
		OpenGL1::BindBuffer( GL_ARRAY_BUFFER, buffer->BufferObj );
		OpenGL1::BufferSubData( GL_ARRAY_BUFFER, a_DstOffsetBytes, a_DataSizeBytes, a_Data );
		OpenGL1::BindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	void RHICommandList_OpenGLImpl::CopyBuffer_Impl( IRHIBuffer& a_DstBuffer, size_t a_DstOffsetBytes, IRHIBuffer& a_SrcBuffer, RHIBufferRange a_SrcRange )
	{
		auto* dstBuffer = a_DstBuffer.As<RHIBuffer_OpenGLImpl>();
		auto* srcBuffer = a_SrcBuffer.As<RHIBuffer_OpenGLImpl>();
		OpenGL1::BindBuffer( GL_COPY_READ_BUFFER, srcBuffer->BufferObj );
		OpenGL1::BindBuffer( GL_COPY_WRITE_BUFFER, dstBuffer->BufferObj );
		OpenGL3::CopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, a_SrcRange.Offset, a_DstOffsetBytes, a_SrcRange.Size );
		OpenGL1::BindBuffer( GL_COPY_READ_BUFFER, 0 );
		OpenGL1::BindBuffer( GL_COPY_WRITE_BUFFER, 0 );
	}

	void RHICommandList_OpenGLImpl::UpdateTexture_Impl( IRHITexture& a_Texture, const RHITextureSlice& a_DstSlice, RHITextureSubresourceData a_Data )
	{
		auto* texture = a_Texture.As<RHITexture_OpenGLImpl>();
		NOT_IMPLEMENTED;
	}

	void RHICommandList_OpenGLImpl::CopyTexture_Impl( IRHITexture& a_DstTexture, const RHITextureSlice& a_DstSlice, IRHITexture& a_SrcTexture, const RHITextureSlice& a_SrcSlice )
	{
		NOT_IMPLEMENTED;
	}

	void RHICommandList_OpenGLImpl::SetInlinedConstants_Impl( const void* a_Data, uint32_t a_SizeBytes, uint32_t a_DstOffsetBytes )
	{
		RHI_DEV_CHECK( m_GraphicsStateValid || m_ComputeStateValid, "Cannot set inlined constants without a valid graphics or compute state!" );

		uint32_t bindingPoint = RHIShaderBinding::InvalidSlot;
		if ( m_GraphicsStateValid )
		{
			bindingPoint = m_CurrentGraphicsState.PipelineState->As<RHIGraphicsPipelineState_OpenGLImpl>()->UniformLayout.InlinedConstants.BindingPoint;
		}
		else if ( m_ComputeStateValid )
		{
			NOT_IMPLEMENTED;
		}

		if ( bindingPoint == RHIShaderBinding::InvalidSlot )
		{
			RHI_DEV_CHECK( false, "Inlined constants uniform location is invalid!" );
			return;
		}

		OpenGL1::BindBuffer( GL_UNIFORM_BUFFER, m_InlinedConstantsUBO );
		OpenGL1::BufferSubData( GL_UNIFORM_BUFFER, a_DstOffsetBytes, a_SizeBytes, a_Data );
		OpenGL1::BindBuffer( GL_UNIFORM_BUFFER, 0 );
		OpenGL4::BindBufferBase( GL_UNIFORM_BUFFER, bindingPoint, m_InlinedConstantsUBO );
	}

	void RHICommandList_OpenGLImpl::SetGraphicsState_Impl( const RHIGraphicsState& a_GraphicsState )
	{
		auto* pso = a_GraphicsState.PipelineState->As<RHIGraphicsPipelineState_OpenGLImpl>();
		RHI_DEV_CHECK( pso, "Invalid graphics pipeline state!" );

		const bool updateFramebuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.Framebuffer != a_GraphicsState.Framebuffer;
		const bool updatePipelineState = !m_GraphicsStateValid || m_CurrentGraphicsState.PipelineState != a_GraphicsState.PipelineState;
		const bool updateIndexBuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.IndexBuffer != a_GraphicsState.IndexBuffer;
		const bool updateVertexBuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.VertexBuffer != a_GraphicsState.VertexBuffer;

		if ( updatePipelineState )
			BindGraphicsPipelineState( *pso );

		if ( updateFramebuffer )
			BindFramebuffer( a_GraphicsState.Framebuffer );

		BindGraphicsBindings( a_GraphicsState );

		OpenGL3::BindVertexArray( pso->GetVAO() );

		if ( updateIndexBuffer )
		{
			auto* indexBuffer = a_GraphicsState.IndexBuffer ? a_GraphicsState.IndexBuffer->As<RHIBuffer_OpenGLImpl>() : nullptr;
			if ( indexBuffer )
				OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer->BufferObj );
			else
				OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		if ( updateVertexBuffer )
		{
			auto* vertexBuffer = a_GraphicsState.VertexBuffer ? a_GraphicsState.VertexBuffer->As<RHIBuffer_OpenGLImpl>() : nullptr;
			if ( vertexBuffer )
				OpenGL1::BindBuffer( GL_ARRAY_BUFFER, vertexBuffer->BufferObj );
			else
				OpenGL1::BindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		pso->ApplyVertexLayoutToVAO( pso->GetVAO() );

		m_CurrentGraphicsState = a_GraphicsState;
		m_GraphicsStateValid = true;
	}

	void RHICommandList_OpenGLImpl::ClearRenderTargets_Impl( ERHIClearFlags a_Flags, Color a_ClearColor, float a_DepthValue, uint8_t a_StencilValue, int32_t a_ColorAttachmentIndex )
	{
		RHI_DEV_CHECK( m_GraphicsStateValid, "Cannot clear render targets without a valid graphics state!" );

		if ( EnumFlags( a_Flags ).HasFlag( ERHIClearFlags::Color ) )
		{
			if ( a_ColorAttachmentIndex >= 0 && a_ColorAttachmentIndex < RHIConstants::MaxColorTargets )
			{
				OpenGL3::ClearBufferfv( GL_COLOR, a_ColorAttachmentIndex, &a_ClearColor[0] );
			}
			else
			{
				OpenGL3::ClearColor( a_ClearColor[0], a_ClearColor[1], a_ClearColor[2], a_ClearColor[3] );
				OpenGL3::Clear( GL_COLOR_BUFFER_BIT );
			}
		}

		if ( EnumFlags( a_Flags ).HasFlag( ERHIClearFlags::Depth ) )
		{
			OpenGL4::ClearDepthf( a_DepthValue );
			OpenGL3::Clear( GL_DEPTH_BUFFER_BIT );
		}

		if ( EnumFlags( a_Flags ).HasFlag( ERHIClearFlags::Stencil ) )
		{
			OpenGL3::ClearStencil( a_StencilValue );
			OpenGL3::Clear( GL_STENCIL_BUFFER_BIT );
		}
	}

	void RHICommandList_OpenGLImpl::SetViewportState_Impl( const RHIViewportState& a_ViewportState )
	{
		RHI_DEV_CHECK( m_GraphicsStateValid, "Cannot set viewport state without a valid graphics state!" );
		// Viewports
		for ( uint32_t i = 0; i < a_ViewportState.Viewports.Size(); ++i )
		{
			const RHIViewport& vp = a_ViewportState.Viewports[i];
			OpenGL4::ViewportIndexedf( i, vp.X, vp.Y, vp.Width, vp.Height );
		}

		// Scissors
		for ( uint32_t i = 0; i < a_ViewportState.Scissors.Size(); ++i )
		{
			const RHIScissorRect& scissor = a_ViewportState.Scissors[i];
			const GLint left = scissor.Left;
			const GLint bottom = scissor.Bottom;
			const GLsizei width = scissor.Right - scissor.Left;
			const GLsizei height = scissor.Bottom - scissor.Top; TODO( "This works for now but is shonky" );
			OpenGL4::ScissorIndexed( i, left, bottom, width, height );
		}
	}

	void RHICommandList_OpenGLImpl::Draw_Impl( const RHIDrawArgs& a_DrawArgs )
	{
		RHI_DEV_CHECK( m_GraphicsStateValid, "Cannot draw without a valid graphics state!" );

		const ERHITopology primitiveTopology = m_CurrentGraphicsState.PipelineState->Desc().Topology;
		const auto* const indexBuffer = m_CurrentGraphicsState.IndexBuffer ? m_CurrentGraphicsState.IndexBuffer->As<RHIBuffer_OpenGLImpl>() : nullptr;
		const auto* const vertexBuffer = m_CurrentGraphicsState.VertexBuffer ? m_CurrentGraphicsState.VertexBuffer->As<RHIBuffer_OpenGLImpl>() : nullptr;

		RHI_DEV_CHECK( !a_DrawArgs.IsIndexed() || indexBuffer, "Cannot draw indexed without a valid index buffer - Drawing without an index buffer" );

		if ( a_DrawArgs.IsIndexed() && indexBuffer )
		{
			// Indexed Draw
			const GLenum indexType = GLTextureFormat::From( indexBuffer->Desc().Format ).Type;
			const void* indexBufferOffset = indexType == GL_UNSIGNED_SHORT ? 
				ReinterpretCast<const void*>( a_DrawArgs.BaseIndex * sizeof( uint16_t ) ) : 
				ReinterpretCast<const void*>( a_DrawArgs.BaseIndex * sizeof( uint32_t ) );
			RHI_DEV_CHECK( indexType == GL_UNSIGNED_SHORT || indexType == GL_UNSIGNED_INT, "Invalid index buffer format! Must be either R16_UINT or R32_UINT." );

			if ( a_DrawArgs.InstanceCount > 1 )
			{
				// Instanced Draw
				OpenGL4::DrawElementsInstancedBaseVertexBaseInstance(
					Translate( primitiveTopology ),
					a_DrawArgs.IndexCount,
					indexType,
					indexBufferOffset,
					a_DrawArgs.InstanceCount,
					a_DrawArgs.BaseVertex,
					a_DrawArgs.BaseInstance
				);
			}
			else
			{
				// Draw
				OpenGL4::DrawElementsBaseVertex(
					Translate( primitiveTopology ),
					a_DrawArgs.IndexCount,
					indexType,
					indexBufferOffset,
					a_DrawArgs.BaseVertex
				);
			}
		}
		else
		{
			// Non-indexed Draw
			if ( a_DrawArgs.InstanceCount > 1 )
			{
				// Instanced Draw
				OpenGL4::DrawArraysInstancedBaseInstance(
					Translate( primitiveTopology ),
					a_DrawArgs.BaseVertex,
					a_DrawArgs.VertexCount,
					a_DrawArgs.InstanceCount,
					a_DrawArgs.BaseInstance
				);
			}
			else
			{
				// Draw
				OpenGL4::DrawArrays(
					Translate( primitiveTopology ),
					a_DrawArgs.BaseVertex,
					a_DrawArgs.VertexCount
				);
			}
		}
	}

	// Debug group commands

	void RHICommandList_OpenGLImpl::PushDebugGroup_Impl( StringView a_Name )
	{
		OpenGL4::PushDebugGroup( GL_DEBUG_SOURCE_APPLICATION, 0, a_Name.size(), a_Name.data() );
	}

	void RHICommandList_OpenGLImpl::PopDebugGroup_Impl()
	{
		OpenGL4::PopDebugGroup();
	}

	void RHICommandList_OpenGLImpl::InsertDebugMarker_Impl( StringView a_Name )
	{
		OpenGL4::DebugMessageInsert( GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, a_Name.size(), a_Name.data() );
	}

	void RHICommandList_OpenGLImpl::FlushCommandBuffer()
	{
		for ( const CommandBuffer::Command& cmdVariant : m_Deferred.CommandBuffer.Commands )
		{
			using enum CommandBuffer::CommandType;
			switch ( Cast<CommandBuffer::CommandType>( cmdVariant.index() ) )
			{
			case UpdateBuffer:
			{
				const auto& cmd = std::get<CommandBuffer::UpdateBuffer>( cmdVariant );
				UpdateBuffer_Impl( *cmd.Buffer, cmd.Data, cmd.DataSizeBytes, cmd.DstOffsetBytes );
				break;
			}
			case CopyBuffer:
			{
				const auto& cmd = std::get<CommandBuffer::CopyBuffer>( cmdVariant );
				CopyBuffer_Impl( *cmd.DstBuffer, cmd.DstOffsetBytes, *cmd.SrcBuffer, cmd.SrcRange );
				break;
			}
			case UpdateTexture:
			{
				const auto& cmd = std::get<CommandBuffer::UpdateTexture>( cmdVariant );
				UpdateTexture_Impl( *cmd.Texture, cmd.DstSlice, cmd.Data );
				break;
			}
			case CopyTexture:
			{
				const auto& cmd = std::get<CommandBuffer::CopyTexture>( cmdVariant );
				CopyTexture_Impl( *cmd.DstTexture, cmd.DstSlice, *cmd.SrcTexture, cmd.SrcSlice );
				break;
			}
			case SetInlinedConstants:
			{
				const auto& cmd = std::get<CommandBuffer::SetInlinedConstants>( cmdVariant );
				SetInlinedConstants_Impl( cmd.Data.Data(), cmd.SizeBytes, cmd.DstOffsetBytes);
				break;
			}
			case SetGraphicsState:
			{
				const auto& cmd = std::get<CommandBuffer::SetGraphicsState>( cmdVariant );
				SetGraphicsState_Impl( cmd.GraphicsState );
				break;
			}
			case ClearRenderTargets:
			{
				const auto& cmd = std::get<CommandBuffer::ClearRenderTargets>( cmdVariant );
				ClearRenderTargets_Impl( cmd.Flags, cmd.ClearColor, cmd.DepthValue, cmd.StencilValue, cmd.ColorAttachmentIndex );
				break;
			}
			case SetViewportState:
			{
				const auto& cmd = std::get<CommandBuffer::SetViewportState>( cmdVariant );
				SetViewportState_Impl( cmd.Viewports );
				break;
			}
			case Draw:
			{
				const auto& cmd = std::get<CommandBuffer::Draw>( cmdVariant );
				Draw_Impl( cmd.DrawArgs );
				break;
			}
			case PushDebugGroup:
			{
				const auto& cmd = std::get<CommandBuffer::PushDebugGroup>( cmdVariant );
				PushDebugGroup_Impl( cmd.Name );
				break;
			}
			case PopDebugGroup:
			{
				PopDebugGroup_Impl();
				break;
			}
			case InsertDebugMarker:
			{
				const auto& cmd = std::get<CommandBuffer::InsertDebugMarker>( cmdVariant );
				InsertDebugMarker_Impl( cmd.Name );
				break;
			}
#if RHI_DEBUG_ENABLED
			default:
				ENSURE( false, "Unknown command type in command buffer!" );
				break;
#endif
			}
		}

		m_Deferred.CommandBuffer.Clear();
	}

#if 0

	void RHICommandList_OpenGLImpl::SetBindingLayout( const RHICommand::SetBindingLayout& a_Data )
	{
		if ( a_Data.SBL == nullptr ) [[unlikely]]
		{
			ASSERT( false, "Attempting to bind a null shader binding layout!" );
			return;
		}

		GLState::s_BoundSBL = SharedPtrCast<RHIBindingLayout_OpenGLImpl>( a_Data.SBL->shared_from_this() );
	}

	void RHICommandList_OpenGLImpl::SetShaderBindings( const RHICommand::SetShaderBindings& a_Data )
	{
		SharedPtr<RHIGraphicsPipelineState_OpenGLImpl> gpso = GLState::s_BoundGraphicsPSO.lock();
		if ( !ASSERT( gpso, "No graphics pipeline state bound!" ) )
			return;

		auto* sbl = a_Data.BindingSet->Desc().Layout->As<RHIBindingLayout_OpenGLImpl>();
		auto* bindingSet = a_Data.BindingSet->As<RHIBindingSet_OpenGLImpl>();

		for ( auto& binding : bindingSet->Desc().Bindings )
		{
			switch ( binding.Type )
			{
			case ERHIBindingType::InlinedConstants:
			{
				// This is handled by SetInlinedConstants
				break;
			}
			case ERHIBindingType::ConstantBuffer:
			{
				RHIBuffer_OpenGLImpl* buffer = binding.Resource->As<RHIBuffer_OpenGLImpl>();
				if ( binding.Range == RHIBufferRange::EntireBuffer() )
				{
					OpenGL4::BindBufferBase( GL_UNIFORM_BUFFER, binding.Slot, buffer->BufferObj );
				}
				else
				{
					OpenGL4::BindBufferRange( GL_UNIFORM_BUFFER, binding.Slot, buffer->BufferObj, binding.Range.Offset, binding.Range.Size );
				}
				break;
			}
			case ERHIBindingType::StructuredBuffer:
				NOT_IMPLEMENTED;
				break;
			case ERHIBindingType::StorageBuffer:
				NOT_IMPLEMENTED;
				break;
			case ERHIBindingType::Texture:
			{
				ASSERT( false, "OpenGL requires textures and samplers to be combined in the shader! - Use a Texture binding instead and set the sampler in the texture." );
				break;
			}
			case ERHIBindingType::StorageTexture:
				NOT_IMPLEMENTED;
				break;
			case ERHIBindingType::Sampler:
			{
				ASSERT( false, "OpenGL requires textures and samplers to be combined in the shader! - Use a Texture binding instead and set the sampler in the texture." );
				break;
			}
			case ERHIBindingType::CombinedSampler:
			{
				// The names of Combined Samplers in GLSL have been set to the Texture name ( from HLSL )
				// So we can just bind the texture and sampler together
				RHITexture_OpenGLImpl* texture = binding.Resource->As<RHITexture_OpenGLImpl>();
				if ( !(texture->Sampler) )
				{
					ASSERT( false, "Texture has no sampler! - OpenGL requires Textures to have a Sampler, you can set the sampler on the IRHITexture." );
					break;
				}

				RHISampler_OpenGLImpl* sampler = texture->Sampler->As<RHISampler_OpenGLImpl>();
				OpenGL4::BindTextureUnit( binding.Slot, texture->TextureObj );
				OpenGL4::BindSampler( binding.Slot, sampler->GetGLHandle() );
				break;
			}
			default:
				ASSERT( false, "Unknown shader binding type!" );
				return;
			}
		}
		
	}

	void RHICommandList_OpenGLImpl::SetInlinedConstants( const RHICommand::SetInlinedConstants& a_Data )
	{
		OpenGL1::BindBuffer( GL_UNIFORM_BUFFER, m_InlinedConstantsUBO );
		OpenGL1::BufferSubData( GL_UNIFORM_BUFFER, a_Data.Range.Offset, a_Data.Range.Size, a_Data.Data.Data() );
		OpenGL1::BindBuffer( GL_UNIFORM_BUFFER, 0 );

		TODO( "Location is always 0, will this always be true?" );
		const GLuint location = 0;
		OpenGL3::BindBufferBase( GL_UNIFORM_BUFFER, location, m_InlinedConstantsUBO );
	}

	void RHICommandList_OpenGLImpl::ResourceBarrier( const RHICommand::ResourceBarrier& a_Data )
	{
		//NOT_IMPLEMENTED;
	}

	void RHICommandList_OpenGLImpl::SetGraphicsPipelineState( const RHICommand::SetGraphicsPipelineState& a_Data )
	{
		if ( a_Data.PSO == nullptr )
		{
			GLState::s_BoundGraphicsPSO.reset();
			return;
		}

		RHIGraphicsPipelineState_OpenGLImpl* pso = a_Data.PSO->As<RHIGraphicsPipelineState_OpenGLImpl>();
		if ( !GLState::s_BoundGraphicsPSO.expired() && GLState::s_BoundGraphicsPSO.lock().get() == pso )
		{
			return;
		}

		GLState::s_BoundGraphicsPSO = SharedPtrCast<RHIGraphicsPipelineState_OpenGLImpl>( a_Data.PSO->shared_from_this() );

		// Bind the VAO
		GLState::BindVertexArray( pso->GetVAO() );

		// Set blend state
		for ( uint32_t i = 0; i < RHIConstants::MaxColorTargets; ++i )
		{
			const RHIBlendState& blendState = a_Data.PSO->Desc().BlendState;
			if ( blendState.Enabled )
			{
				OpenGL3::Enablei( GL_BLEND, i );
				OpenGL4::BlendFuncSeparatei( i, 
					OpenGL::Translate( blendState.SrcColor ),
					OpenGL::Translate( blendState.DstColor ),
					OpenGL::Translate( blendState.SrcAlpha ),
					OpenGL::Translate( blendState.DstAlpha ) );
				OpenGL4::BlendEquationi( i, OpenGL::Translate( blendState.BlendEquation ) );
			}
			else
			{
				OpenGL3::Disablei( GL_BLEND, i );
			}
		}

		// Set depth state
		const RHIDepthState& depthState = a_Data.PSO->Desc().DepthState;
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
		const RHIStencilState& stencilState = a_Data.PSO->Desc().StencilState;
		if ( stencilState.Enabled )
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
		const RHIRasterizerState& rasterizerState = a_Data.PSO->Desc().RasterizerState;
		switch ( rasterizerState.CullMode )
		{
		case ERHICullMode::None:
			OpenGL3::Disable( GL_CULL_FACE );
			break;
		case ERHICullMode::Front:
			OpenGL3::Enable( GL_CULL_FACE );
			OpenGL3::CullFace( GL_FRONT );
			break;
		case ERHICullMode::Back:
			OpenGL3::Enable( GL_CULL_FACE );
			OpenGL3::CullFace( GL_BACK );
			break;
		default: ASSERT( false, "Invalid cull mode in Graphics Pipeline State!" ); break;
		}
		switch ( rasterizerState.FillMode )
		{
		case ERHIFillMode::Point:
			OpenGL3::PolygonMode( GL_FRONT_AND_BACK, GL_POINT );
			break;
		case ERHIFillMode::Solid:
			OpenGL3::PolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			break;
		case ERHIFillMode::Wireframe:
			OpenGL3::PolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			break;
		default: ASSERT( false, "Invalid fill mode in Graphics Pipeline State!" ); break;
		}
		OpenGL3::FrontFace( rasterizerState.Clockwise ? GL_CW : GL_CCW );


		// Bind the shader program last to avoid unnecessary state changes
		GLState::BindProgram( pso->GetShaderProgramID() );
	}

	void RHICommandList_OpenGLImpl::SetRenderTargets( const RHICommand::SetRenderTargets& a_Data )
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
			RHITexture_OpenGLImpl* rtv = a_Data.RTV[i]->As<RHITexture_OpenGLImpl>();
			OpenGL3::FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, rtv->TextureObj, mipmapLevelToRenderTo );
		}

		// Bind the depth stencil target
		if ( a_Data.DSV )
		{
			RHITexture_OpenGLImpl* dsv = a_Data.DSV->As<RHITexture_OpenGLImpl>();
			const bool hasStencil = GetRHIFormatInfo( dsv->Desc().Format ).HasStencil;
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

			ASSERT( false, "Framebuffer is not complete!" );
			return;
		}

		// Set the draw buffers
		constexpr GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
		OpenGL3::DrawBuffers( a_Data.RTV.Size(), drawBuffers );
	}

	void RHICommandList_OpenGLImpl::ClearRenderTargets( const RHICommand::ClearRenderTargets& a_Data )
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

	void RHICommandList_OpenGLImpl::SetScissors( const RHICommand::SetScissors& a_Data )
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

	void RHICommandList_OpenGLImpl::SetViewports( const RHICommand::SetViewports& a_Data )
	{
		//OpenGL1::Viewport( a_Data.Viewports[0].X, a_Data.Viewports[0].Y, a_Data.Viewports[0].Width, a_Data.Viewports[0].Height );
		for ( uint32_t i = 0; i < a_Data.Viewports.Size(); ++i )
		{
			const RHIViewport& vp = a_Data.Viewports[i];
			OpenGL4::ViewportIndexedf( i, vp.X, vp.Y, vp.Width, vp.Height );
		}
	}

	void RHICommandList_OpenGLImpl::SetIndexBuffer( const RHICommand::SetIndexBuffer& a_Data )
	{
	}

	void RHICommandList_OpenGLImpl::SetVertexBuffer( const RHICommand::SetVertexBuffer& a_Data )
	{
		if ( a_Data.VBO == nullptr )
		{
			GLState::s_BoundVBO = 0;
		}
		else
		{
			GLState::s_BoundVBO = a_Data.VBO->As<RHIBuffer_OpenGLImpl>()->BufferObj;
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

	void RHICommandList_OpenGLImpl::SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology& a_Data )
	{
		GLState::s_BoundPrimitiveTopology = Translate( a_Data.Topology );
	}

	void RHICommandList_OpenGLImpl::Draw( const RHICommand::Draw& a_Data )
	{
		OpenGL1::DrawArrays( GLState::s_BoundPrimitiveTopology, a_Data.VertexStart, a_Data.VertexCount );
	}

	void RHICommandList_OpenGLImpl::DrawIndexed( const RHICommand::DrawIndexed& a_Data )
	{
		TODO( "Fix index start i fink" );
		OpenGL2::DrawElements( GLState::s_BoundPrimitiveTopology, a_Data.IndexCount, GL_UNSIGNED_INT, ReinterpretCast<const void*>( a_Data.IndexStart ) );
	}

	void RHICommandList_OpenGLImpl::SetComputePipelineState( const RHICommand::SetComputePipelineState& a_Data )
	{
	}

	void RHICommandList_OpenGLImpl::DispatchCompute( const RHICommand::DispatchCompute& a_Data )
	{
	}

	void RHICommandList_OpenGLImpl::DispatchComputeIndirect( const RHICommand::DispatchComputeIndirect& a_Data )
	{
	}

#endif

} // namespace Tridium
