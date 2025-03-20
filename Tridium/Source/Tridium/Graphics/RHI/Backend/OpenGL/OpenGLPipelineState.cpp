#include "tripch.h"
#include "OpenGLPipelineState.h"
#include "OpenGLShader.h"

namespace Tridium {

    bool OpenGLGraphicsPipelineState::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHIGraphicsPipelineStateDescriptor>( a_Params );
		if ( !desc )
			return false;

		// Create the shader program
		{
			m_ShaderProgramID = OpenGL4::CreateProgram();
			if ( !m_ShaderProgramID )
				return false;

			// Attach the shaders
			static constexpr auto AttachShader = +[]( const RHIShaderModuleRef& a_Shader, GLuint a_ProgramID )
				{
					if ( a_Shader )
					{
						OpenGLShaderModule* shader = a_Shader->As<OpenGLShaderModule>();
						OpenGL4::AttachShader( a_ProgramID, shader->GetGLHandle() );
					}
				};

			AttachShader( desc->VertexShader, m_ShaderProgramID );
			AttachShader( desc->HullShader, m_ShaderProgramID );
			AttachShader( desc->DomainShader, m_ShaderProgramID );
			AttachShader( desc->GeometryShader, m_ShaderProgramID );
			AttachShader( desc->PixelShader, m_ShaderProgramID );

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
				return false;
			}
		#endif

			// Validate the program
			OpenGL4::ValidateProgram( m_ShaderProgramID );

			// Detach the shaders
			static constexpr auto DetachShader = +[]( const RHIShaderModuleRef& a_Shader, GLuint a_ProgramID )
				{
					if ( a_Shader )
					{
						OpenGLShaderModule* shader = a_Shader->As<OpenGLShaderModule>();
						OpenGL4::DetachShader( a_ProgramID, shader->GetGLHandle() );
					}
				};

			DetachShader( desc->VertexShader, m_ShaderProgramID );
			DetachShader( desc->HullShader, m_ShaderProgramID );
			DetachShader( desc->DomainShader, m_ShaderProgramID );
			DetachShader( desc->GeometryShader, m_ShaderProgramID );
			DetachShader( desc->PixelShader, m_ShaderProgramID );
		}

		// Create the VAO
		{
			OpenGL3::GenVertexArrays( 1, &m_VAO );
		}

		// Collect the uniform locations
		{
			m_UnifromLocations.clear();
			for ( const RHIShaderBinding& binding : desc->ShaderBindingLayout->GetDescriptor()->Bindings )
			{
				const auto InitUniform = [&]( StringView a_Name )
					{
						GLint location = OpenGL4::GetUniformLocation( m_ShaderProgramID, a_Name.data() );
						if ( location < 0 )
						{
							LOG( LogCategory::RHI, Error, "Uniform '{0}' not found in shader while committing PSO '{1}'", a_Name, desc->Name );
						}
						else
						{
							m_UnifromLocations[binding.Name] = location;
						}
					};

				if ( binding.IsInlined() )
				{
					for ( const auto& [name, tensorType] : binding.InlinedConstant->Tensors )
					{
						InitUniform( name );
					}
				}
				if ( binding.IsReferencedSamplers() || binding.IsReferencedTextures() )
				{
					// OpenGL combines samplers and textures into a single binding
					m_UnifromLocations[binding.Name] = -1;
				}
				else
				{
					InitUniform( binding.Name.String() );
				}
			}
		}

		// Collect combined samplers
		{
			m_CombinedSamplers.clear();
			static constexpr auto CollectCombinedSamplers = +[]( const RHIShaderModuleRef& a_Shader, UnorderedMap<StringView, Array<StringView>>& a_CombinedSamplers )
				{
					if ( a_Shader )
					{
						OpenGLShaderModule* shader = a_Shader->As<OpenGLShaderModule>();
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

		return true;
    }

	bool OpenGLGraphicsPipelineState::Release()
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

	bool OpenGLGraphicsPipelineState::IsValid() const
	{
		return m_ShaderProgramID != 0;
	}

	bool OpenGLGraphicsPipelineState::ApplyVertexLayoutToVAO( GLuint a_VAO )
	{
		OpenGLState::BindVertexArray( a_VAO ); // Ensure the VAO is bound

		// Bind the vertex layout
		for ( uint32_t i = 0; i < GetDescriptor()->VertexLayout.Elements.Size(); ++i )
		{
			const RHIVertexAttribute& element = GetDescriptor()->VertexLayout.Elements[i];
			OpenGLVertexElementType type = ToOpenGL::GetVertexElementType( element.Type );
			if ( !type.Valid() )
			{
				ASSERT_LOG( false, "Invalid vertex element type" );
				return false;
			}

			const uint32_t stride = GetDescriptor()->VertexLayout.Stride;
			OpenGL2::EnableVertexAttribArray( i );
			OpenGL2::VertexAttribPointer( i, type.Count, type.Type, type.Normalized, stride, reinterpret_cast<const void*>( element.Offset ) );
		}

		return true;
	}

}
