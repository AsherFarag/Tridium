#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	RHIGraphicsPipelineState_OpenGLImpl::RHIGraphicsPipelineState_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHIGraphicsPipelineState( a_Device, a_Desc )
    {
		// Create the shader program
		{
			m_ShaderProgramID = OpenGL4::CreateProgram();
			if ( !ASSERT( m_ShaderProgramID != 0, "Failed to create shader program!" ) )
				return;

			// Attach the shaders
			static constexpr auto AttachShader = +[]( const RHIShaderModuleRef& a_Shader, GLuint a_ProgramID )
				{
					if ( a_Shader )
					{
						RHIShaderModule_OpenGLImpl* shader = a_Shader->As<RHIShaderModule_OpenGLImpl>();
						OpenGL4::AttachShader( a_ProgramID, shader->GetGLHandle() );
					}
				};

			AttachShader( a_Desc.VertexShader, m_ShaderProgramID );
			AttachShader( a_Desc.HullShader, m_ShaderProgramID );
			AttachShader( a_Desc.DomainShader, m_ShaderProgramID );
			AttachShader( a_Desc.GeometryShader, m_ShaderProgramID );
			AttachShader( a_Desc.PixelShader, m_ShaderProgramID );

			// Link the program
			OpenGL4::LinkProgram( m_ShaderProgramID );

			// Build the uniform layout
			{
				for ( uint32_t i = 0; i < a_Desc.BindingLayouts.Size(); ++i )
				{
					const auto& layout = a_Desc.BindingLayouts[i];
					if ( layout )
						UniformLayout.SetLayout( i, *layout, m_ShaderProgramID );
				}
			}

		#if RHI_DEBUG_ENABLED
			// Check for linking errors
			GLint success;
			OpenGL4::GetProgramiv( m_ShaderProgramID, GL_LINK_STATUS, &success );
			if ( !success )
			{
				GLchar infoLog[512];
				OpenGL4::GetProgramInfoLog( m_ShaderProgramID, 512, nullptr, infoLog );
				LOG( LogCategory::RHI, Error, "Shader program linking failed: {0}", infoLog );
				return;
			}
		#endif

			// Validate the program
			OpenGL4::ValidateProgram( m_ShaderProgramID );

			// Detach the shaders
			static constexpr auto DetachShader = +[]( const RHIShaderModuleRef& a_Shader, GLuint a_ProgramID )
				{
					if ( a_Shader )
					{
						RHIShaderModule_OpenGLImpl* shader = a_Shader->As<RHIShaderModule_OpenGLImpl>();
						OpenGL4::DetachShader( a_ProgramID, shader->GetGLHandle() );
					}
				};

			DetachShader( a_Desc.VertexShader, m_ShaderProgramID );
			DetachShader( a_Desc.HullShader, m_ShaderProgramID );
			DetachShader( a_Desc.DomainShader, m_ShaderProgramID );
			DetachShader( a_Desc.GeometryShader, m_ShaderProgramID );
			DetachShader( a_Desc.PixelShader, m_ShaderProgramID );
		}

		// Create the VAO
		{
			OpenGL3::GenVertexArrays( 1, &m_VAO );
		}
    }

	bool RHIGraphicsPipelineState_OpenGLImpl::Release()
	{
		if ( m_ShaderProgramID )
		{
			OpenGL4::DeleteProgram( m_ShaderProgramID );
			m_ShaderProgramID = 0;
		}

		if ( m_VAO )
		{
			OpenGL3::DeleteVertexArrays( 1, &m_VAO );
			m_VAO = 0;
		}

		return true;
	}

	bool RHIGraphicsPipelineState_OpenGLImpl::Valid() const
	{
		return m_ShaderProgramID != 0;
	}

	bool RHIGraphicsPipelineState_OpenGLImpl::ApplyVertexLayoutToVAO( GLuint a_VAO ) const
	{
		OpenGL3::BindVertexArray( a_VAO );

		// Bind the vertex layout
		for ( uint32_t i = 0; i < Desc().VertexLayout.Elements.Size(); ++i )
		{
			const RHIVertexAttribute& element = Desc().VertexLayout.Elements[i];
			VertexElementType type = VertexElementType::From( element.Type );
			if ( !type.Valid() )
			{
				ASSERT( false, "Invalid vertex element type" );
				return false;
			}

			const uint32_t stride = Desc().VertexLayout.Stride;
			OpenGL2::EnableVertexAttribArray( i );
			OpenGL2::VertexAttribPointer( i, type.Count, type.Type, type.Normalized, stride, ReinterpretCast<const void*>( element.Offset ) );
		}

		return true;
	}

	GLint RHIGraphicsPipelineState_OpenGLImpl::TryGetUniformLocation( hash_t a_NameHash ) const
	{
		auto it = m_UnifromLocations.find( a_NameHash );
		if ( it != m_UnifromLocations.end() )
		{
			return it->second;
		}
		return -1;
	}

}
