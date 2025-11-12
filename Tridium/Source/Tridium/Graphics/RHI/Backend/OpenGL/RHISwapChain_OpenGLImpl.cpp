#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

    bool RHISwapChain_OpenGLImpl::Present()
    {
		if ( Window )
        {
			// Clear the bound framebuffer
			OpenGL3::BindFramebuffer( GL_FRAMEBUFFER, 0 );

			glDisable( GL_MULTISAMPLE );  // temporarily disable MSAA
			//glEnable( GL_FRAMEBUFFER_SRGB );

			// Render the framebuffer to the screen
			{
				OpenGL1::Viewport( 0, 0, m_Width, m_Height );
				OpenGL1::Scissor( 0, 0, m_Width, m_Height );

				OpenGL1::Disable( GL_DEPTH_TEST );
				
				// Draw the textured quad onto the screen
				OpenGL2::UseProgram( m_Framebuffer.ShaderID );

				OpenGL1::ActiveTexture( GL_TEXTURE0 );
				OpenGL1::BindTexture( GL_TEXTURE_2D, *m_Framebuffer.BackBufferTexture->NativePtrAs<GLuint>() );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				OpenGL1::TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				OpenGL2::Uniform1i( OpenGL2::GetUniformLocation( m_Framebuffer.ShaderID, "u_Texture" ), 0 );

				OpenGL3::BindVertexArray( m_Framebuffer.ScreenQuad.VAO );
				OpenGL3::DrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );
				OpenGL3::BindVertexArray( 0 );
			}

            glfwSwapBuffers( Window );
            return true;
        }

        return false;
    }

	RHITextureRef RHISwapChain_OpenGLImpl::GetBackBuffer()
	{
		return m_Framebuffer.BackBufferTexture;
	}

	bool RHISwapChain_OpenGLImpl::Resize( uint32_t a_Width, uint32_t a_Height )
	{
		if ( !Window || !a_Width || !a_Height )
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

	RHISwapChain_OpenGLImpl::RHISwapChain_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHISwapChain( a_Device, a_Desc )
    {
		m_Width = a_Desc.Width;
		m_Height = a_Desc.Height;

		Window = glfwGetCurrentContext();
		if ( !ASSERT( Window, "Failed to get current OpenGL context!" ) )
		{
			return;
		}

		glfwSwapInterval( a_Desc.Flags.HasFlag( ERHISwapChainFlags::UseVSync ) ? 1 : 0 );

		ASSERT( m_Framebuffer.Init( a_Desc ), "Failed to create OpenGL framebuffer!" );
    }

	bool RHISwapChain_OpenGLImpl::Release()
	{
		Window = nullptr;
		m_Framebuffer = {};
		return true;
	}

	bool Framebuffer::Init( const RHISwapChainDesc& a_Desc )
	{
		// Create the back buffer texture
		const auto texDesc = RHITextureDesc{}
			.SetName( "BackBuffer" )
			.SetDimension( ERHITextureDimension::Texture2D )
			.SetWidth( a_Desc.Width ).SetHeight( a_Desc.Height ).SetDepth( 1 )
			.SetFormat( a_Desc.Format )
			.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource )
			.SetHeapType( ERHIHeapType::Default );

		BackBufferTexture = RHI::CreateTexture( texDesc );
		if ( !ASSERT( BackBufferTexture, "Failed to create back buffer texture!" ) )
		{
			return false;
		}

		// Create VAO, IBO and VBO for the screen quad
		OpenGL3::GenVertexArrays( 1, &ScreenQuad.VAO );

		OpenGL3::GenBuffers( 1, &ScreenQuad.VBO );
		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, ScreenQuad.VBO );
		OpenGL3::BufferData( GL_ARRAY_BUFFER, sizeof( ScreenQuad.Vertices ), ScreenQuad.Vertices, GL_STATIC_DRAW );

		OpenGL3::GenBuffers( 1, &ScreenQuad.IBO );
		OpenGL3::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, ScreenQuad.IBO );
		OpenGL3::BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( ScreenQuad.Indices ), ScreenQuad.Indices, GL_STATIC_DRAW );

		// Bind the VAO
		OpenGL3::BindVertexArray( ScreenQuad.VAO );

		// Bind the VBO
		OpenGL3::BindBuffer( GL_ARRAY_BUFFER, ScreenQuad.VBO );
		// Set the vertex attributes
		OpenGL3::VertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, nullptr );
		OpenGL3::EnableVertexAttribArray( 0 );
		OpenGL3::VertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 5, ReinterpretCast<void*>( sizeof( float ) * 3 ) );
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
			in vec2 v_TexCoord;
			uniform sampler2D u_Texture;

			out vec4 o_Color;
			void main()
			{
				vec2 flippedTexCoord = vec2(v_TexCoord.x, 1.0 - v_TexCoord.y);
				vec3 color = texture(u_Texture, flippedTexCoord).rgb;
				o_Color = vec4(color, 1.0);
			}


		)";

		ShaderID = OpenGL3::CreateProgram();
		uint32_t vertexShader = OpenGL3::CreateShader( GL_VERTEX_SHADER );
		uint32_t fragmentShader = OpenGL3::CreateShader( GL_FRAGMENT_SHADER );

		OpenGL3::ShaderSource( vertexShader, 1, &vertexShaderSource, nullptr );
		OpenGL3::CompileShader( vertexShader );

		// Check for compilation errors
		GLint success;
		OpenGL3::GetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );
		if ( !success )
		{
			GLchar infoLog[512];
			OpenGL3::GetShaderInfoLog( vertexShader, 512, nullptr, infoLog );
			LOG( LogCategory::Rendering, Error, "Failed to compile screen vertex shader: %s", infoLog );
			return false;
		}

		OpenGL3::ShaderSource( fragmentShader, 1, &fragmentShaderSource, nullptr );
		OpenGL3::CompileShader( fragmentShader );

		// Check for compilation errors
		OpenGL3::GetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );
		if ( !success )
		{
			GLchar infoLog[512];
			OpenGL3::GetShaderInfoLog( fragmentShader, 512, nullptr, infoLog );
			LOG( LogCategory::Rendering, Error, "Failed to compile screen fragment shader: %s", infoLog );
			return false;
		}

		OpenGL3::AttachShader( ShaderID, vertexShader );
		OpenGL3::AttachShader( ShaderID, fragmentShader );
		OpenGL3::LinkProgram( ShaderID );

		// Check for linking errors
		OpenGL3::GetProgramiv( ShaderID, GL_LINK_STATUS, &success );
		if ( !success )
		{
			GLchar infoLog[512];
			OpenGL3::GetProgramInfoLog( ShaderID, 512, nullptr, infoLog );
			LOG( LogCategory::Rendering, Error, "Failed to link screen shader program: %s", infoLog );
			return false;
		}

		OpenGL3::DeleteShader( vertexShader );
		OpenGL3::DeleteShader( fragmentShader );

		return true;
	}

	void Framebuffer::Resize( uint32_t a_Width, uint32_t a_Height )
	{
		auto desc = BackBufferTexture->Desc();
		desc.Width = a_Width;
		desc.Height = a_Height;
		BackBufferTexture = RHI::CreateTexture( desc );
	}

} // namespace Tridium