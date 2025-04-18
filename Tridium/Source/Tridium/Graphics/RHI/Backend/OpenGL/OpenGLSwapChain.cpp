#include "tripch.h"
#include "OpenGLSwapChain.h"
#include "OpenGLDynamicRHI.h"
#include "../../RHI.h"

namespace Tridium {

    bool OpenGLSwapChain::Present()
    {
		if ( Window )
        {
			GLState::BindFBO( 0 );

			// Render the framebuffer to the screen
			{
				OpenGL1::Viewport( 0, 0, m_Width, m_Height );
				OpenGL1::ClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
				OpenGL1::Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

				GLState::BindProgram( m_Framebuffer.ShaderID );

				OpenGL4::BindTextureUnit( 0, *m_Framebuffer.BackBufferTexture->NativePtrAs<GLuint>() );

				GLState::BindVertexArray( m_Framebuffer.ScreenQuad.VAO );
				OpenGL3::DrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );
				GLState::BindVertexArray( 0 );
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

	bool OpenGLSwapChain::Resize( uint32_t a_Width, uint32_t a_Height )
	{
		if ( !Window )
		{
			return false;
		}

		if ( a_Width == m_Width && a_Height == m_Height )
		{
			return true;
		}

		m_Width = a_Width;
		m_Height = a_Height;
		m_Framebuffer.Resize( a_Width, a_Height );

		return true;
	}

	bool OpenGLSwapChain::Commit( const RHISwapChainDescriptor& a_Desc )
    {
		m_Desc = a_Desc;

		m_Width = a_Desc.Width;
		m_Height = a_Desc.Height;

		Window = glfwGetCurrentContext();
		if ( !Window )
		{
			return false;
		}

		glfwSwapInterval( a_Desc.Flags.HasFlag( ERHISwapChainFlags::UseVSync ) ? 1 : 0 );

		OpenGLDynamicRHI* rhi = RHI::GetOpenGLRHI();
		if ( !rhi )
		{
			return false;
		}

		if ( !m_Framebuffer.Init( a_Desc ) )
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
		const auto texDesc = RHITextureDescriptor(
			"BackBuffer",
			ERHITextureDimension::Texture2D,
			a_Desc.Width, a_Desc.Height, 1,
			a_Desc.Format
		).SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource );

		BackBufferTexture = RHI::CreateTexture( texDesc );
		if ( !BackBufferTexture )
		{
			return false;
		}

		// Create VAO, IBO and VBO for the screen quad
		OpenGL4::CreateVertexArrays( 1, &ScreenQuad.VAO );

		OpenGL4::CreateBuffers( 1, &ScreenQuad.VBO );
		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, ScreenQuad.VBO );
		OpenGL3::BufferData( GL_ARRAY_BUFFER, sizeof( ScreenQuad.Vertices ), ScreenQuad.Vertices, GL_STATIC_DRAW );

		OpenGL4::CreateBuffers( 1, &ScreenQuad.IBO );
		OpenGL3::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, ScreenQuad.IBO );
		OpenGL3::BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( ScreenQuad.Indices ), ScreenQuad.Indices, GL_STATIC_DRAW );

		// Bind the VAO
		OpenGL3::BindVertexArray( ScreenQuad.VAO );

		// Bind the VBO
		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, ScreenQuad.VBO );
		// Set the vertex attributes
		OpenGL3::VertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, nullptr );
		OpenGL3::EnableVertexAttribArray( 0 );
		OpenGL3::VertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, reinterpret_cast<void*>( sizeof( float ) * 3 ) );
		OpenGL3::EnableVertexAttribArray( 1 );

		// Bind the IBO
		OpenGL3::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, ScreenQuad.IBO );

		OpenGL3::BindVertexArray( 0 );

		// Create Shaders
		static const GLchar* vertexShaderSource = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			out vec2 v_TexCoord;
			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		static const GLchar* fragmentShaderSource = R"(
			#version 330 core
			layout(location = 0) out vec4 color;
			in vec2 v_TexCoord;
			uniform sampler2D u_Texture;
			void main()
			{
				color = vec4( texture(u_Texture, v_TexCoord).rgb, 1.0 );
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

	void OpenGLFramebuffer::Resize( uint32_t a_Width, uint32_t a_Height )
	{
		// We just need to resize the back buffer texture
		OpenGL1::BindTexture( GL_TEXTURE_2D, *BackBufferTexture->NativePtrAs<GLuint>() );
		OpenGL1::TexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, a_Width, a_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );
		OpenGL1::BindTexture( GL_TEXTURE_2D, 0 );
	}

} // namespace Tridium