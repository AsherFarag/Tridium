#include "tripch.h"
#include "OpenGLSwapChain.h"
#include "OpenGLDynamicRHI.h"

namespace Tridium {

    bool OpenGLSwapChain::Present()
    {
		if ( Window )
        {
			OpenGL3::BindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
			OpenGL3::BindFramebuffer( GL_DRAW_FRAMEBUFFER, m_Framebuffer.FramebufferID );

			const RHITextureDescriptor* desc = m_Framebuffer.BackBufferTexture->GetDescriptor();
			OpenGL3::BlitFramebuffer( 
				0, 0, desc->Width, desc->Height,
				0, 0, desc->Width, desc->Height,
				GL_COLOR_BUFFER_BIT, GL_NEAREST );

			OpenGL3::BindFramebuffer( GL_FRAMEBUFFER, 0 );

			// Render the framebuffer to the screen
			{
				OpenGL2::UseProgram( m_Framebuffer.ShaderID );
				OpenGL2::ActiveTexture( GL_TEXTURE0 );
				OpenGL2::BindTexture( GL_TEXTURE_2D, *m_Framebuffer.BackBufferTexture->NativePtrAs<GLuint>() );
				OpenGL2::Uniform1i( OpenGL2::GetUniformLocation( m_Framebuffer.ShaderID, "u_Texture" ), 0 );

				OpenGL3::BindVertexArray( m_Framebuffer.ScreenQuad.VAO );
				OpenGL3::DrawArrays( GL_TRIANGLES, 0, 6 );
				OpenGL3::BindVertexArray( 0 );
			}

            glfwSwapBuffers( Window );
            return true;
        }

        return false;
    }

	RHITextureRef OpenGLSwapChain::GetBackBuffer()
	{
		return m_Framebuffer.BackBufferTexture;
	}

    bool OpenGLSwapChain::Commit( const void* a_Params )
    {
		const auto* desc = ParamsToDescriptor<RHISwapChainDescriptor>( a_Params );
		if ( !desc )
		{
			return false;
		}

		Window = glfwGetCurrentContext();
		if ( !Window )
		{
			return false;
		}

		glfwSwapInterval( desc->Flags.HasFlag( ERHISwapChainFlags::UseVSync ) ? 1 : 0 );

		OpenGLDynamicRHI* rhi = RHI::GetOpenGLRHI();
		if ( !rhi )
		{
			return false;
		}

		if ( !m_Framebuffer.Init( *desc ) )
		{
			return false;
		}

		return true;
    }

	bool OpenGLSwapChain::Release()
	{
		Window = nullptr;
		m_Framebuffer = {};
		return true;
	}

	bool OpenGLFramebuffer::Init( const RHISwapChainDescriptor& a_Desc )
	{
		// Create the back buffer texture
		RHITextureDescriptor textureDesc;
		textureDesc.Width = a_Desc.Width;
		textureDesc.Height = a_Desc.Height;
		textureDesc.Format = ERHITextureFormat::RGBA8;
		textureDesc.UsageHint = ERHIUsageHint::RenderTarget;
		textureDesc.Dimension = ERHITextureDimension::Texture2D;
		textureDesc.IsRenderTarget = true;
		textureDesc.Name = "Back Buffer";
		BackBufferTexture = RHI::GetDynamicRHI()->CreateTexture( textureDesc );
		if ( !BackBufferTexture )
		{
			return false;
		}

		// Create the framebuffer and attach the back buffer texture
		OpenGL3::GenFramebuffers( 1, &FramebufferID );
		OpenGL3::BindFramebuffer( GL_FRAMEBUFFER, FramebufferID );
		OpenGL3::FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *BackBufferTexture->NativePtrAs<GLuint>(), 0 );

		// Create VAO and VBO for the screen quad
		OpenGL3::GenVertexArrays( 1, &ScreenQuad.VAO );
		OpenGL3::GenBuffers( 1, &ScreenQuad.VBO );
		OpenGL3::BindVertexArray( ScreenQuad.VAO );

		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, ScreenQuad.VBO );
		OpenGL3::BufferData( GL_ARRAY_BUFFER, sizeof( ScreenQuad.Vertices ), ScreenQuad.Vertices, GL_STATIC_DRAW );

		OpenGL3::EnableVertexAttribArray( 0 );
		OpenGL3::VertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), (void*)0 );
		OpenGL3::EnableVertexAttribArray( 1 );
		OpenGL3::VertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), (void*)( 2 * sizeof( float ) ) );

		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, 0 );
		OpenGL3::BindVertexArray( 0 );

		// Create Shaders
		static const GLchar* vertexShaderSource = R"(
			#version 330 core
			layout(location = 0) in vec2 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			out vec2 v_TexCoord;
			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = vec4(a_Position, 0.0, 1.0);
			}
		)";

		static const GLchar* fragmentShaderSource = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			in vec2 v_TexCoord;
			uniform sampler2D u_Texture;
			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}
		)";

		ShaderID = OpenGL3::CreateProgram();
		uint32_t vertexShader = OpenGL3::CreateShader( GL_VERTEX_SHADER );
		uint32_t fragmentShader = OpenGL3::CreateShader( GL_FRAGMENT_SHADER );

		OpenGL3::ShaderSource( vertexShader, 1, &vertexShaderSource, nullptr );
		OpenGL3::CompileShader( vertexShader );

		OpenGL3::ShaderSource( fragmentShader, 1, &fragmentShaderSource, nullptr );
		OpenGL3::CompileShader( fragmentShader );

		OpenGL3::AttachShader( ShaderID, vertexShader );
		OpenGL3::AttachShader( ShaderID, fragmentShader );
		OpenGL3::LinkProgram( ShaderID );

		OpenGL3::DeleteShader( vertexShader );
		OpenGL3::DeleteShader( fragmentShader );

		return true;
	}

} // namespace Tridium