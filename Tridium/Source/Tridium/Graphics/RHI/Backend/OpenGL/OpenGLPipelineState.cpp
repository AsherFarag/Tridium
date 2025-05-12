#include "tripch.h"
#include "OpenGLPipelineState.h"
#include "OpenGLShader.h"

namespace Tridium {

	RHIGraphicsPipelineState_OpenGLImpl::RHIGraphicsPipelineState_OpenGLImpl( const DescriptorType& a_Desc )
		: RHIGraphicsPipelineState( a_Desc )
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

		// Collect the uniform locations
		{
			m_UnifromLocations.clear();
			for ( const RHIShaderBinding& binding : a_Desc.BindingLayout->Descriptor().Bindings )
			{
				const auto InitUniform = [&]( StringView a_Name )
					{
						GLint location = OpenGL4::GetUniformLocation( m_ShaderProgramID, a_Name.data() );
						if ( location < 0 )
						{
							LOG( LogCategory::RHI, Error, "Uniform '{0}' not found in shader while committing PSO '{1}'", a_Name, a_Desc.Name );
						}
						else
						{
							m_UnifromLocations[binding.NameHash] = location;
						}
					};

				if ( binding.Type() == ERHIShaderBindingType::Sampler )
				{
					// OpenGL combines samplers and textures into a single binding
					m_UnifromLocations[binding.NameHash] = -1;
				}
				else
				{
					InitUniform( m_Desc.BindingLayout->Descriptor().GetBindingName( binding.NameHash ) );
				}
			}
		}

		// Collect combined samplers
		{
			TODO( "the hell is this" );
			m_CombinedSamplers.clear();
			static constexpr auto CollectCombinedSamplers = +[]( const RHIShaderModuleRef& a_Shader, UnorderedMap<StringView, Array<StringView>>& a_CombinedSamplers )
				{
					if ( a_Shader )
					{
						RHIShaderModule_OpenGLImpl* shader = a_Shader->As<RHIShaderModule_OpenGLImpl>();
						//for ( const auto& [texName, samplerNames] : shader->GetCombinedSamplers() )
						//{
						//	Array<StringView>& samplers = a_CombinedSamplers[texName];
						//	samplers.Reserve( samplers.Size() + samplerNames.size() );
						//	//for ( const StringView& sampler : samplerNames )
						//	//{
						//	//	samplers.PushBack( sampler );
						//	//}
						//}
					}
				};
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

	bool RHIGraphicsPipelineState_OpenGLImpl::IsValid() const
	{
		return m_ShaderProgramID != 0;
	}

	bool RHIGraphicsPipelineState_OpenGLImpl::ApplyVertexLayoutToVAO( GLuint a_VAO )
	{
		GLState::BindVertexArray( a_VAO ); // Ensure the VAO is bound

		// Bind the vertex layout
		for ( uint32_t i = 0; i < Descriptor().VertexLayout.Elements.Size(); ++i )
		{
			const RHIVertexAttribute& element = Descriptor().VertexLayout.Elements[i];
			OpenGLVertexElementType type = ToOpenGL::GetVertexElementType( element.Type );
			if ( !type.Valid() )
			{
				ASSERT( false, "Invalid vertex element type" );
				return false;
			}

			const uint32_t stride = Descriptor().VertexLayout.Stride;
			OpenGL2::EnableVertexAttribArray( i );
			OpenGL2::VertexAttribPointer( i, type.Count, type.Type, type.Normalized, stride, reinterpret_cast<const void*>( element.Offset ) );
		}

		return true;
	}

}
