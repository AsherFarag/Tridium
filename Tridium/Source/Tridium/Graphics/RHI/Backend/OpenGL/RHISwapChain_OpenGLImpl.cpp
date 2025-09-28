#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	static void ResetOpenGLState( const int32_t a_Width, const int32_t a_Height )
	{
		// Viewport & scissor
		glViewport( 0, 0, a_Width, a_Height );
		glScissor( 0, 0, a_Width, a_Height );
		glDisable( GL_SCISSOR_TEST );

		// Framebuffer binding
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		glEnable( GL_FRAMEBUFFER_SRGB );

		// Depth / stencil
		glDisable( GL_DEPTH_TEST );
		glDepthMask( GL_TRUE );
		glDepthFunc( GL_LESS );
		glDisable( GL_STENCIL_TEST );
		glStencilMask( 0xFF );
		glStencilFunc( GL_ALWAYS, 0, 0xFF );
		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

		// Blending
		glDisable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ZERO );
		glBlendEquation( GL_FUNC_ADD );

		// Rasterizer state
		glDisable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		glFrontFace( GL_CCW );

		// Color writes
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

		// Polygon mode
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

		// Line width / point size (set back to sane defaults)
		glLineWidth( 1.0f );
		glPointSize( 1.0f );

		// Active texture unit
		glActiveTexture( GL_TEXTURE0 );

		// Unbind VAO, buffers, program, texture
		glBindVertexArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_UNIFORM_BUFFER, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glUseProgram( 0 );
	}

	// Simple checkerboard texture
	GLuint MakeTestTexture()
	{
		const int W = 2, H = 2;
		unsigned char pixels[ W * H * 3 ] = {
			255,   0,   0,    0, 255,   0,
			  0, 255,   0,  255,   0,   0
		};

		GLuint tex;
		glGenTextures( 1, &tex );
		glBindTexture( GL_TEXTURE_2D, tex );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );


		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		glBindTexture( GL_TEXTURE_2D, 0 );

		return tex;
	}

	// Create fullscreen quad VAO/VBO/EBO
	GLuint CreateFullscreenQuad()
	{
		static const float vertices[] = {
			// positions   // texcoords
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f
		};

		static const uint32_t indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLuint VAO, VBO, EBO;
		glGenVertexArrays( 1, &VAO );
		glGenBuffers( 1, &VBO );
		glGenBuffers( 1, &EBO );

		glBindVertexArray( VAO );

		glBindBuffer( GL_ARRAY_BUFFER, VBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

		// Position
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), ( void* )0 );
		glEnableVertexAttribArray( 0 );

		// TexCoord
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), ( void* )( 3 * sizeof( float ) ) );
		glEnableVertexAttribArray( 1 );

		glBindVertexArray( 0 );
		return VAO;
	}


	void Present( GLuint framebufferColorTex, GLuint a_QuadVAO, GLuint screenShader, int width, int height )
	{
		// Bind default framebuffer (the actual window)
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glViewport( 0, 0, width, height );

		// Clear to see if the draw works
		glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		// State setup for fullscreen quad
		glDisable( GL_DEPTH_TEST );
		glDisable( GL_STENCIL_TEST );
		glDisable( GL_BLEND );

		// Use screen blit shader
		glUseProgram( screenShader );

		static GLuint testTex = MakeTestTexture();
		static GLuint quadVAO = CreateFullscreenQuad();

		// Bind FBO color texture to unit 0
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, testTex );
		glUniform1i( glGetUniformLocation( screenShader, "u_Texture" ), 0 );

		// Draw fullscreen quad
		glBindVertexArray( quadVAO );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		// Unbind
		glBindVertexArray( 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glUseProgram( 0 );
	}


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
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, *m_Framebuffer.BackBufferTexture->NativePtrAs<GLuint>() );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
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
			.SetUsage( ERHIUsage::Dynamic );

		BackBufferTexture = RHI::CreateTexture( texDesc );
		if ( !ASSERT( BackBufferTexture, "Failed to create back buffer texture!" ) )
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