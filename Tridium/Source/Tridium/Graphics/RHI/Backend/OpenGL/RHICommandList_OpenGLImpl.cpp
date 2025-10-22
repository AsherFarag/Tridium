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
		IRHICommandList::Open();

		// Clear
		m_Deferred.CommandBuffer.Clear();
		m_CurrentGraphicsState = {};
		m_GraphicsStateValid = false;

		return true;
	}

	bool RHICommandList_OpenGLImpl::Close()
	{
		IRHICommandList::Close();
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

	void RHICommandList_OpenGLImpl::ClearTexture( IRHITexture& a_Texture, const RHITextureSubresourceSet& a_Subresources, RHIClearValue a_ClearValue, ERHIClearFlags a_ClearFlags, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::ClearTexture( a_Texture, a_Subresources, a_ClearValue, a_ClearFlags, RHI_DEBUG_SRC_LOC );

		m_ReferencedObjects.EmplaceBack( a_Texture.SharedFromThis() );

		if ( IsImmediate() )
		{
			ClearTexture_Impl( a_Texture, a_Subresources, a_ClearValue, a_ClearFlags );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::ClearTexture{ 
				.Texture = &a_Texture, 
				.Subresources = a_Subresources, 
				.ClearValue = a_ClearValue, 
				.Flags = a_ClearFlags 
			} );
		}
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

	void RHICommandList_OpenGLImpl::SetGraphicsState( const RHIGraphicsState& a_GraphicsState, bool a_ClearViewportState, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::SetGraphicsState( a_GraphicsState, a_ClearViewportState, RHI_DEBUG_SRC_LOC );

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
			SetGraphicsState_Impl( a_GraphicsState, a_ClearViewportState );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::SetGraphicsState{ 
				.GraphicsState = a_GraphicsState,
				.ClearViewportState = a_ClearViewportState
			} );
		}
	}

	void RHICommandList_OpenGLImpl::ClearRenderTargets( ERHIClearFlags a_Flags, RHIClearValue a_ClearValue, int32_t a_ColorAttachmentIndex, RHI_DEBUG_SRC_LOC_PARAM )
	{
		IRHICommandList::ClearRenderTargets( a_Flags, a_ClearValue, a_ColorAttachmentIndex, RHI_DEBUG_SRC_LOC );
		if ( IsImmediate() )
		{
			ClearRenderTargets_Impl( a_Flags, a_ClearValue, a_ColorAttachmentIndex );
		}
		else
		{
			m_Deferred.CommandBuffer.Commands.EmplaceBack( CommandBuffer::ClearRenderTargets{ 
				.Flags = a_Flags, 
				.ClearValue = a_ClearValue,
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

		// Bind color attachments
		for ( size_t i = 0; i < a_Framebuffer.ColorAttachments.Size(); ++i )
		{
			const auto& attachment = a_Framebuffer.ColorAttachments[i];
			if ( !attachment.Texture )
			{
				continue;
			}

			const auto* texture = attachment.Texture->As<RHITexture_OpenGLImpl>();
			const GLenum target = attachment.ReadOnly ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER;
			const int mipmapLevelToRenderTo = attachment.Slice.MipLevel;
			switch ( texture->Desc().Dimension )
			{
				case ERHITextureDimension::Texture2D:
				{
					OpenGL3::FramebufferTexture2D( target, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->GLHandle(), mipmapLevelToRenderTo );
					break;
				}
				case ERHITextureDimension::TextureCube:
				{
					const GLenum cubeFaceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + attachment.Slice.ArraySlice;
					OpenGL3::FramebufferTexture2D( target, GL_COLOR_ATTACHMENT0 + i, cubeFaceTarget, texture->GLHandle(), mipmapLevelToRenderTo );
					break;
				}
				default:
				{
					ASSERT( false, "Unsupported texture dimension for framebuffer color attachment!" );
					break;
				}
			}
		}

		if ( a_Framebuffer.DepthStencilAttachment )
		{
			const auto* depthStencilTexture = a_Framebuffer.DepthStencilAttachment.Texture->As<RHITexture_OpenGLImpl>();
			const GLenum target = a_Framebuffer.DepthStencilAttachment.ReadOnly ? GL_READ_FRAMEBUFFER : GL_FRAMEBUFFER;
			const GLenum attachmentType = GetRHIFormatInfo( a_Framebuffer.DepthStencilAttachment.Texture->Desc().Format ).HasStencil
				? GL_DEPTH_STENCIL_ATTACHMENT
				: GL_DEPTH_ATTACHMENT;
			const int mipmapLevelToRenderTo = a_Framebuffer.DepthStencilAttachment.Slice.MipLevel;
			OpenGL3::FramebufferTexture2D( target, attachmentType, GL_TEXTURE_2D, depthStencilTexture->GLHandle(), mipmapLevelToRenderTo );
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
		uint32_t textureUnitCounter = 0;

		// Iterate over each binding set in the graphics state
		for ( uint32_t bindingSetIndex = 0; bindingSetIndex < a_GraphicsState.BindingSets.Size(); ++bindingSetIndex )
		{
			const auto* bindingSet = Cast<const RHIBindingSet_OpenGLImpl*>( a_GraphicsState.BindingSets[bindingSetIndex] );
			if ( bindingSet == nullptr )
				continue;

			const IRHIBindingLayout* bindingLayout = bindingSet->Desc().Layout.get();
			const RHIBindingSetItemArray& bindings = bindingSet->Desc().Bindings;
			RHI_DEV_CHECK( bindingLayout, "Binding set {0} has no layout!", bindingSetIndex );

			// Iterate over each binding in the layout and bind the corresponding resource
			// If a binding is not found in the binding set, bind a null resource
			for ( const auto& bindingDesc : bindingLayout->Desc().Bindings )
			{
				// Find the corresponding uniform in the pipeline's uniform layout
				const Uniform* uniformPtr = uniformLayout.GetUniform( bindingSetIndex, bindingDesc.Type(), bindingDesc.Slot );
				if ( uniformPtr == nullptr )
				{
					continue;
				}

				const Uniform& uniform = *uniformPtr;
				const GLint bindingPoint = uniform.BindingPoint;

				if ( bindingPoint < 0 )
				{
					LOG( LogCategory::Debug, Warn, "Binding point for binding set {0}, slot {1} is invalid ({2})! Skipping binding.",
						bindingSetIndex, bindingDesc.Slot, bindingPoint );
					continue; // Invalid binding point, skip this binding
				}

				bool isBufferBinding = false;
				bool isTextureBinding = false;
				GLenum bufferTarget = 0;

				switch ( bindingDesc.Type() )
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
					default:
						RHI_DEV_CHECK( false, "Unknown binding type {0} for binding set {1}, binding {2}!",
							ToString( bindingDesc.Type() ), bindingSetIndex, bindingDesc.Slot );
						break;
				}

				bool found = false;

				for ( const RHIBindingSetItem& binding : bindings )
				{
					if ( binding.Slot != bindingDesc.Slot )
					{
						 // Only consider bindings for this slot
						continue;
					}

					if ( isBufferBinding && binding.Type == bindingDesc.Type() )
					{
						if ( auto* buffer = binding.Resource->As<RHIBuffer_OpenGLImpl>() )
						{
							if ( binding.Range.IsEntireBuffer() )
							{
								OpenGL3::BindBufferBase( bufferTarget, bindingPoint, buffer->BufferObj );
							}
							else
							{
								OpenGL3::BindBufferRange( bufferTarget, bindingPoint, buffer->BufferObj, binding.Range.Offset, binding.Range.Size );
							}

							found = true;
							break; // Found the binding for this slot
						}
					}
					else if ( isTextureBinding && binding.Type == bindingDesc.Type() )
					{
						GLuint unit = textureUnitCounter++;
						if ( auto* texture = binding.Resource->As<RHITexture_OpenGLImpl>() )
						{
							// Bind texture directly to unit
							OpenGL4::BindTextureUnit( unit, texture->GLHandle() );

							// Choose sampler
							RHISampler sampler = binding.Sampler.Valid()
								? binding.Sampler.Unpack()
								: texture->Desc().DefaultSampler;
							const bool isDepth = GetRHIFormatInfo( binding.Format ).HasDepth;
							GLuint glSampler = Device()->ResourceCache().GetOrCreateSampler( sampler, isDepth );

							// Bind sampler to same unit
							OpenGL4::BindSampler( unit, glSampler );
							OpenGL2::Uniform1i( bindingPoint, unit );

							found = true;
							break; // Found the binding for this slot
						}
					}
				}

				// If we didn't find a matching binding, bind a null resource
				if ( !found )
				{
					if ( isBufferBinding )
					{
						OpenGL3::BindBufferBase( bufferTarget, bindingPoint, 0 );
					}
					else if ( isTextureBinding )
					{
						// Use null texture
						GLuint nullTexture = *Device()->ResourceCache().NullTexture2D->NativePtrAs<GLuint>();
						GLuint unit = textureUnitCounter++;
						OpenGL4::BindTextureUnit( unit, nullTexture );

						// Use fallback sampler
						GLuint glSampler = Device()->ResourceCache().GetOrCreateSampler( RHISampler{}, false );
						OpenGL4::BindSampler( unit, glSampler );
						OpenGL2::Uniform1i( bindingPoint, unit );
					}
				}
			}
		}
	}

	//
	// === Command Implementations ===
	//

	void RHICommandList_OpenGLImpl::ClearTexture_Impl( IRHITexture& a_Texture, const RHITextureSubresourceSet& a_Subresources, RHIClearValue a_ClearValue, ERHIClearFlags a_ClearFlags )
	{
		auto* texture = a_Texture.As<RHITexture_OpenGLImpl>();
		const auto& desc = texture->Desc();
		const auto subresources = a_Subresources.Resolve( desc, false );
		const bool isRenderTarget = EnumFlags( texture->Desc().BindFlags ).HasFlag( ERHIBindFlags::RenderTarget );
		const bool isDepthStencil = EnumFlags( texture->Desc().BindFlags ).HasFlag( ERHIBindFlags::DepthStencil );
		const bool isUAV = EnumFlags( texture->Desc().BindFlags ).HasFlag( ERHIBindFlags::UnorderedAccess );

		if ( !isRenderTarget && !isDepthStencil && !isUAV )
		{
			RHI_DEV_CHECK( false, "Clearing a texture that is neither a render target, depth-stencil, or UAV!" );
			return;
		}

		const auto CalcMipSize = +[]( uint32_t a_Size, uint32_t a_MipLevel ) -> GLint
		{
			return Cast<GLint>( Math::Max( a_Size >> a_MipLevel, 1u ) );
		};

		// Clear UAVs with glClearTexSubImage
		if ( isUAV && !isRenderTarget && !isDepthStencil )
		{
			for ( uint32_t mip = subresources.BaseMipLevel; mip < subresources.BaseMipLevel + subresources.NumMipLevels; ++mip )
			{
				const GLint w = CalcMipSize( desc.Width, mip );
				const GLint h = CalcMipSize( desc.Height, mip );

				for ( uint32_t layer = subresources.BaseArraySlice; layer < subresources.BaseArraySlice + subresources.NumArraySlices; ++layer )
				{
					OpenGL4::ClearTexSubImage(
						texture->GLHandle(),
						Cast<GLint>( mip ),
						0, 0, Cast<GLint>( layer ),
						w, h, 1,
						texture->GLFormat().Format,
						texture->GLFormat().Type,
						&a_ClearValue.Color[0]
					);
				}
			}

			return;
		}

		GLint prevFBO = 0;
		OpenGL3::GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevFBO);

		GLuint scratchFBO = 0;
		OpenGL3::GenFramebuffers(1, &scratchFBO);
		OpenGL3::BindFramebuffer(GL_FRAMEBUFFER, scratchFBO);

		// Precompute whether we can attach the whole level (all array slices)
		const bool isArray = desc.IsArray();
		const bool attachWholeLevelIfRequested = isArray &&
		    subresources.BaseArraySlice == 0 &&
			subresources.NumArraySlices == desc.DepthOrArraySize;

		// Determine clear mask / values for depth/stencil and color
		const bool clearDepth   = EnumFlags( a_ClearFlags ).HasFlag( ERHIClearFlags::Depth );
		const bool clearStencil = EnumFlags( a_ClearFlags ).HasFlag( ERHIClearFlags::Stencil );
		const bool clearColor   = EnumFlags( a_ClearFlags ).HasFlag( ERHIClearFlags::Color ) || isRenderTarget;

		// Color4 clears: set clear color once
		if ( clearColor && isRenderTarget )
		{
			OpenGL1::ColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE ); // Ensure color mask allows writing
		    OpenGL1::ClearColor( a_ClearValue.Color[0], a_ClearValue.Color[1], a_ClearValue.Color[2], a_ClearValue.Color[3] );
		}

		const RHIFormatInfo formatInfo = GetRHIFormatInfo( desc.Format );

		// Depth / Stencil clears: set values once
		if ( isDepthStencil )
		{
			if ( clearDepth )
			{
				OpenGL1::DepthMask( GL_TRUE ); // Ensure depth mask allows writing
				OpenGL4::ClearDepthf( 1.0 /* a_ClearValue.Depth */);
			}

			if ( clearStencil && formatInfo.HasStencil )
			{
				OpenGL1::StencilMask( 0xFF ); // Ensure stencil mask allows writing
				OpenGL1::ClearStencil( Cast<GLint>( a_ClearValue.Stencil ) );
			}
		}

		const GLenum attachment = isDepthStencil
			? ( formatInfo.HasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT )
			: GL_COLOR_ATTACHMENT0;

		// Attach & clear loop
		for ( uint32_t mip = subresources.BaseMipLevel; mip < subresources.BaseMipLevel + subresources.NumMipLevels; ++mip )
		{
			if ( attachWholeLevelIfRequested )
		    {
		        // Attach the entire level (layered framebuffer). This may allow clearing all layers at once.
		        // Use glFramebufferTexture to attach all layers at the given mip level.
				OpenGL3::FramebufferTexture( GL_FRAMEBUFFER, attachment, texture->GLHandle(), Cast<GLint>( mip ) );

		        // Check completeness
				const GLenum status = OpenGL3::CheckFramebufferStatus( GL_FRAMEBUFFER );
				if ( status == GL_FRAMEBUFFER_COMPLETE )
		        {
		            GLbitfield mask = 0;

					if ( isRenderTarget && clearColor )
					{
						mask |= GL_COLOR_BUFFER_BIT;
					}

		            if (isDepthStencil)
		            {
		                if (clearDepth)   mask |= GL_DEPTH_BUFFER_BIT;
		                if (clearStencil) mask |= GL_STENCIL_BUFFER_BIT;
		            }

		            if (mask != 0)
					{
						OpenGL1::Viewport( 0, 0, CalcMipSize( desc.Width, mip ), CalcMipSize( desc.Height, mip ) );
						OpenGL1::Scissor( 0, 0, CalcMipSize( desc.Width, mip ), CalcMipSize( desc.Height, mip ) );
						OpenGL1::Clear( mask );
					}
		        }

		        // Detach the attachment to avoid keeping references (optional but tidy)
		        // Re-attach null by calling FramebufferTexture with 0
				OpenGL3::FramebufferTexture( GL_FRAMEBUFFER, attachment, 0, 0 );
		    }
		    else
		    {
		        // Per-layer / per-slice clearing (attach each layer individually).
				for ( uint32_t layer = subresources.BaseArraySlice; layer < subresources.BaseArraySlice + subresources.NumArraySlices; ++layer )
		        {
					// Attach the layer
					GLenum target = texture->GLTarget();
					if ( target == GL_TEXTURE_2D_ARRAY || target == GL_TEXTURE_3D )
					{
						// Layered types
						OpenGL3::FramebufferTextureLayer( GL_FRAMEBUFFER, attachment, texture->GLHandle(), mip, layer );
					}
					else if ( target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY )
					{
						// Cubemaps: select a face via arraySlice or subresource mapping
						GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
						OpenGL3::FramebufferTexture2D( GL_FRAMEBUFFER, attachment, face, texture->GLHandle(), mip );
					}
					else
					{
						// Regular 2D texture
						OpenGL3::FramebufferTexture2D( GL_FRAMEBUFFER, attachment, target, texture->GLHandle(), mip );
					}


		            const GLenum status = OpenGL3::CheckFramebufferStatus(GL_FRAMEBUFFER);
					if ( RHI_DEV_CHECK( status == GL_FRAMEBUFFER_COMPLETE,
										"Framebuffer incomplete when trying to clear texture! Status: {}", status ) )
		            {
		                GLbitfield mask = 0;

						if ( isRenderTarget && clearColor )
						{
							mask |= GL_COLOR_BUFFER_BIT;
						}

						if ( isDepthStencil )
		                {
							if ( clearDepth ) mask |= GL_DEPTH_BUFFER_BIT;
							if ( clearStencil && formatInfo.HasStencil ) mask |= GL_STENCIL_BUFFER_BIT;
		                }

						if ( mask != 0 )
						{
							OpenGL1::Viewport( 0, 0, CalcMipSize( desc.Width, mip ), CalcMipSize( desc.Height, mip ) );
							OpenGL1::Scissor( 0, 0, CalcMipSize( desc.Width, mip ), CalcMipSize( desc.Height, mip ) );
							OpenGL1::Clear( mask );
						}
		            }

		            // Detach the layer
					if ( target == GL_TEXTURE_2D_ARRAY || target == GL_TEXTURE_3D )
						OpenGL3::FramebufferTextureLayer( GL_FRAMEBUFFER, attachment, 0, 0, 0 );
					else
						OpenGL3::FramebufferTexture2D( GL_FRAMEBUFFER, attachment, 0, 0, 0 );
		        }
		    }
		}

		// Restore previous FBO binding and delete scratch FBO
		OpenGL3::BindFramebuffer( GL_FRAMEBUFFER, Cast<GLuint>( prevFBO ) );
		OpenGL3::DeleteFramebuffers( 1, &scratchFBO );

		// Reset viewport and scissor to previous state
		bool gfxStateValid = m_GraphicsStateValid;
		TODO( "Dirty hack because SetViewportState_Impl checks if gfx state is valid and Im too lazy to fix it properly" );
		m_GraphicsStateValid = true;
		SetViewportState_Impl( m_ViewportState );
		m_GraphicsStateValid = gfxStateValid;
	}

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
		const auto& desc = texture->Desc();
		RHITextureSlice dstSlice = a_DstSlice.Resolve( desc );
		if ( desc.Is1D() )
		{
			ScopedTextureBinding textureBinding( GL_TEXTURE_1D, texture->GLHandle() );
			OpenGL1::TexSubImage1D(
				GL_TEXTURE_1D, dstSlice.MipLevel,
				dstSlice.OffsetX,
				dstSlice.Width,
				texture->GLFormat().Format, texture->GLFormat().Type,
				a_Data.Data );
		}
		else if ( desc.Is2D() )
		{
			ScopedTextureBinding textureBinding( GL_TEXTURE_2D, texture->GLHandle() );
			OpenGL1::TexSubImage2D( 
				GL_TEXTURE_2D, dstSlice.MipLevel,
				dstSlice.OffsetX, dstSlice.OffsetY,
				dstSlice.Width, dstSlice.Height,
				texture->GLFormat().Format, texture->GLFormat().Type,
				a_Data.Data );
		}
		else if ( desc.Is3D() )
		{
			ScopedTextureBinding textureBinding( GL_TEXTURE_3D, texture->GLHandle() );
			OpenGL1::TexSubImage3D(
				GL_TEXTURE_3D, dstSlice.MipLevel,
				dstSlice.OffsetX, dstSlice.OffsetY, dstSlice.OffsetZ,
				dstSlice.Width, dstSlice.Height, dstSlice.Depth,
				texture->GLFormat().Format, texture->GLFormat().Type,
				a_Data.Data );
		}
		else if ( desc.IsCube() )
		{
			const uint8_t* dataPtr = Cast<const uint8_t*>( a_Data.Data );
			ScopedTextureBinding textureBinding( GL_TEXTURE_CUBE_MAP, texture->GLHandle() );
			for ( uint32_t face = 0; face < 6; ++face )
			{
				const uint8_t* pixels = dataPtr + face * a_Data.RowStride * dstSlice.Height;
				OpenGL1::TexSubImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, dstSlice.MipLevel,
					dstSlice.OffsetX, dstSlice.OffsetY,
					dstSlice.Width, dstSlice.Height,
					texture->GLFormat().Format, texture->GLFormat().Type,
					pixels );
			}
		}
		else
		{
			RHI_DEV_CHECK( false, "Unsupported texture type for UpdateTexture!" );
			return;
		}
	}

	void RHICommandList_OpenGLImpl::CopyTexture_Impl( IRHITexture& a_DstTexture, const RHITextureSlice& a_DstSlice, IRHITexture& a_SrcTexture, const RHITextureSlice& a_SrcSlice )
	{
		auto* dstTexture = a_DstTexture.As<RHITexture_OpenGLImpl>();
		auto* srcTexture = a_SrcTexture.As<RHITexture_OpenGLImpl>();

		const GLenum dstTarget = dstTexture->GLTarget();
		const GLenum srcTarget = srcTexture->GLTarget();

		const auto dstSlice = a_DstSlice.Resolve( dstTexture->Desc() );
		const auto srcSlice = a_SrcSlice.Resolve( srcTexture->Desc() );
		const size_t width = Math::Min( dstSlice.Width, srcSlice.Width );
		const size_t height = Math::Min( dstSlice.Height, srcSlice.Height );
		const size_t depth = Math::Min( dstSlice.Depth, srcSlice.Depth );

		//ScopedTextureBinding dstBinding( dstTarget, dstTexture->GLHandle() );
		//ScopedTextureBinding srcBinding( srcTarget, srcTexture->GLHandle() );

		if ( dstTarget == GL_TEXTURE_1D )
		{
			OpenGL4::CopyImageSubData(
				srcTexture->GLHandle(), GL_TEXTURE_1D, srcSlice.MipLevel,
				srcSlice.OffsetX, 0, 0,
				dstTexture->GLHandle(), GL_TEXTURE_1D, dstSlice.MipLevel,
				dstSlice.OffsetX, 0, 0,
				width, 1, 1 );
		}
		else if ( dstTarget == GL_TEXTURE_2D )
		{
			OpenGL4::CopyImageSubData(
				srcTexture->GLHandle(), GL_TEXTURE_2D, srcSlice.MipLevel,
				srcSlice.OffsetX, srcSlice.OffsetY, 0,
				dstTexture->GLHandle(), GL_TEXTURE_2D, dstSlice.MipLevel,
				dstSlice.OffsetX, dstSlice.OffsetY, 0,
				width, height, 1 );
		}
		else if ( dstTarget == GL_TEXTURE_3D )
		{
			OpenGL4::CopyImageSubData(
				srcTexture->GLHandle(), GL_TEXTURE_3D, srcSlice.MipLevel,
				srcSlice.OffsetX, srcSlice.OffsetY, srcSlice.OffsetZ,
				dstTexture->GLHandle(), GL_TEXTURE_3D, dstSlice.MipLevel,
				dstSlice.OffsetX, dstSlice.OffsetY, dstSlice.OffsetZ,
				width, height, depth );
		}
		else if ( dstTarget == GL_TEXTURE_CUBE_MAP )
		{
			for ( uint32_t face = 0; face < 6; ++face )
			{
				OpenGL4::CopyImageSubData(
					srcTexture->GLHandle(), GL_TEXTURE_CUBE_MAP, srcSlice.MipLevel,
					srcSlice.OffsetX, srcSlice.OffsetY, face,
					dstTexture->GLHandle(), GL_TEXTURE_CUBE_MAP, dstSlice.MipLevel,
					dstSlice.OffsetX, dstSlice.OffsetY, face,
					width, height, 1 );
			}
		}
		else if ( dstTarget == GL_BUFFER )
		{
			OpenGL1::BindBuffer( GL_PIXEL_PACK_BUFFER, dstTexture->GLHandle() );

			const GLenum srcTarget = srcTexture->GLTarget();
			const auto& glFormat = srcTexture->GLFormat();

			const GLint mip = srcSlice.MipLevel;
			const GLsizei width = static_cast<GLsizei>( srcSlice.Width );
			const GLsizei height = static_cast<GLsizei>( srcSlice.Height );
			const GLsizei depth = static_cast<GLsizei>( srcSlice.Depth );

			const uint32_t bytesPerPixel = GetRHIFormatInfo( srcTexture->Desc().Format ).Bytes();

			// Compute the offset into the PBO
			const GLsizei dstOffset = dstTexture->GetSubresourceRange( dstSlice ).Offset;

			OpenGL4::GetTextureSubImage(
				srcTexture->GLHandle(),
				mip,
				srcSlice.OffsetX,
				srcSlice.OffsetY,
				srcSlice.OffsetZ + srcSlice.ArraySlice,
				width,
				height,
				depth,
				glFormat.Format,
				glFormat.Type,
				dstTexture->GetTotalSizeInBytes(),
				(void*)( dstOffset )
			);

			OpenGL1::BindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
		}
		else
		{
			RHI_DEV_CHECK( false, "Unsupported texture type for CopyTexture!" );
			return;
		}
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
		OpenGL3::BindBufferBase( GL_UNIFORM_BUFFER, bindingPoint, m_InlinedConstantsUBO );
	}

	void RHICommandList_OpenGLImpl::SetGraphicsState_Impl( const RHIGraphicsState& a_GraphicsState, bool a_ClearViewportState )
	{
		auto* pso = a_GraphicsState.PipelineState->As<RHIGraphicsPipelineState_OpenGLImpl>();
		RHI_DEV_CHECK( pso, "Invalid graphics pipeline state!" );

		const bool updateFramebuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.Framebuffer != a_GraphicsState.Framebuffer;
		const bool updatePipelineState = !m_GraphicsStateValid || m_CurrentGraphicsState.PipelineState != a_GraphicsState.PipelineState;
		const bool updateIndexBuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.IndexBuffer != a_GraphicsState.IndexBuffer;
		const bool updateVertexBuffer = !m_GraphicsStateValid || m_CurrentGraphicsState.VertexBuffer != a_GraphicsState.VertexBuffer;

		if ( updatePipelineState )
		{
			BindGraphicsPipelineState( *pso );
		}

		if ( updateFramebuffer )
		{
			BindFramebuffer( a_GraphicsState.Framebuffer );
		}

		if ( a_ClearViewportState )
		{
			OpenGL1::Viewport( 0, 0, 0, 0 );
			OpenGL1::Scissor( 0, 0, 0, 0 );
			m_ViewportState = RHIViewportState{};
		}
		else
		{
			SetViewportState_Impl( m_ViewportState );
		}

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

	void RHICommandList_OpenGLImpl::ClearRenderTargets_Impl( ERHIClearFlags a_Flags, RHIClearValue a_ClearValue, int32_t a_ColorAttachmentIndex )
	{
		RHI_DEV_CHECK( m_GraphicsStateValid, "Cannot clear render targets without a valid graphics state!" );

		if ( EnumFlags( a_Flags ).HasFlag( ERHIClearFlags::Color ) )
		{
			if ( a_ColorAttachmentIndex >= 0 && a_ColorAttachmentIndex < RHIConstants::MaxColorTargets )
			{
				OpenGL3::ClearBufferfv( GL_COLOR, a_ColorAttachmentIndex, &a_ClearValue.Color[0] );
			}
			else
			{
				OpenGL3::ClearColor( a_ClearValue.Color[0], a_ClearValue.Color[1], a_ClearValue.Color[2], a_ClearValue.Color[3] );
				OpenGL3::Clear( GL_COLOR_BUFFER_BIT );
			}
		}

		if ( EnumFlags( a_Flags ).HasFlag( ERHIClearFlags::Depth ) )
		{
			OpenGL4::ClearDepthf( a_ClearValue.Depth );
			OpenGL3::Clear( GL_DEPTH_BUFFER_BIT );
		}

		if ( EnumFlags( a_Flags ).HasFlag( ERHIClearFlags::Stencil ) )
		{
			OpenGL3::ClearStencil( a_ClearValue.Stencil );
			OpenGL3::Clear( GL_STENCIL_BUFFER_BIT );
		}
	}

	void RHICommandList_OpenGLImpl::SetViewportState_Impl( const RHIViewportState& a_ViewportState )
	{
		RHI_DEV_CHECK( m_GraphicsStateValid, "Cannot set viewport state without a valid graphics state!" );

		m_ViewportState = a_ViewportState;

		// Set viewports
		for ( uint32_t i = 0; i < a_ViewportState.Viewports.Size(); ++i )
		{
			const RHIViewport& vp = a_ViewportState.Viewports[i];
			OpenGL4::ViewportIndexedf( i, vp.X, vp.Y, vp.Width, vp.Height );
		}

		// Set scissors (if any)
		const bool scissorsEnabled = !a_ViewportState.Scissors.Empty();
		if ( scissorsEnabled )
		{
			OpenGL1::Enable( GL_SCISSOR_TEST );

			for ( uint32_t i = 0; i < a_ViewportState.Scissors.Size(); ++i )
			{
				const RHIScissorRect& scissor = a_ViewportState.Scissors[ i ];
				// We already flip the Y coordinate in the shader, so no need to do it here.
				// Not anymore, so we need to flip it again here
				const GLint flippedTop = m_CurrentGraphicsState.Framebuffer ?
					m_CurrentGraphicsState.Framebuffer.ColorAttachments[0].Texture->Desc().Height - ( scissor.Top + scissor.Height() ) :
					0;

				OpenGL4::ScissorIndexed( i, scissor.Left, flippedTop, scissor.Width(), scissor.Height() );
			}
		}
		else
		{
			OpenGL1::Disable( GL_SCISSOR_TEST );
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
			case ClearTexture:
			{
				const auto& cmd = std::get<CommandBuffer::ClearTexture>( cmdVariant );
				ClearTexture_Impl( *cmd.Texture, cmd.Subresources, cmd.ClearValue, cmd.Flags );
				break;
			}
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
				SetGraphicsState_Impl( cmd.GraphicsState, cmd.ClearViewportState );
				break;
			}
			case ClearRenderTargets:
			{
				const auto& cmd = std::get<CommandBuffer::ClearRenderTargets>( cmdVariant );
				ClearRenderTargets_Impl( cmd.Flags, cmd.ClearValue, cmd.ColorAttachmentIndex );
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

} // namespace Tridium
