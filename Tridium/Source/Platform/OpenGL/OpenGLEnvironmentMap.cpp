#include "tripch.h"
#include "OpenGLEnvironmentMap.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Core/Application.h>
#include <Tridium/Rendering/RenderCommand.h>
#include <Tridium/Rendering/Framebuffer.h>

namespace Tridium {

	static Matrix4 s_CaptureProjection = glm::perspective( glm::radians( 90.0f ), 1.0f, 0.1f, 10.0f );
	static Matrix4 s_CaptureVies[] =
	{
		glm::lookAt( Vector3(0.0f), Vector3( 1.0f,  0.0f,  0.0f ),  Vector3( 0.0f, -1.0f,  0.0f ) ),
		glm::lookAt( Vector3(0.0f), Vector3( -1.0f,  0.0f,  0.0f ), Vector3( 0.0f, -1.0f,  0.0f ) ),
		glm::lookAt( Vector3(0.0f), Vector3( 0.0f,  1.0f,  0.0f ),  Vector3( 0.0f,  0.0f,  1.0f ) ),
		glm::lookAt( Vector3(0.0f), Vector3( 0.0f, -1.0f,  0.0f ),  Vector3( 0.0f,  0.0f, -1.0f ) ),
		glm::lookAt( Vector3(0.0f), Vector3( 0.0f,  0.0f,  1.0f ),  Vector3( 0.0f, -1.0f,  0.0f ) ),
		glm::lookAt( Vector3(0.0f), Vector3( 0.0f,  0.0f, -1.0f ),  Vector3( 0.0f, -1.0f,  0.0f ) )
	};

	void RenderCube()
	{
		static const float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			 // bottom face
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 // top face
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};

		static unsigned int cubeVAO = 0;
		static unsigned int cubeVBO = 0;

		if ( cubeVAO == 0 )
		{
			// Generate and bind VAO and VBO once
			glGenVertexArrays( 1, &cubeVAO );
			glGenBuffers( 1, &cubeVBO );

			// Same setup as before (binding buffer, uploading data, setting attributes)
			glBindBuffer( GL_ARRAY_BUFFER, cubeVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

			glBindVertexArray( cubeVAO );
			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), (void*)0 );
			glEnableVertexAttribArray( 1 );
			glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), (void*)( 3 * sizeof( float ) ) );
			glEnableVertexAttribArray( 2 );
			glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), (void*)( 6 * sizeof( float ) ) );

			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );
		}

		// Render the cube
		glBindVertexArray( cubeVAO );
		glDrawArrays( GL_TRIANGLES, 0, 36 );
		glBindVertexArray( 0 );
	}

	void RenderQuad()
	{
		static unsigned int quadVAO = 0;
		static unsigned int quadVBO;
		if ( quadVAO == 0 )
		{
			// Setup plane VAO
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			glGenVertexArrays( 1, &quadVAO );
			glGenBuffers( 1, &quadVBO );
			glBindVertexArray( quadVAO );
			glBindBuffer( GL_ARRAY_BUFFER, quadVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), &quadVertices, GL_STATIC_DRAW );
			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void*)0 );
			glEnableVertexAttribArray( 1 );
			glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void*)( 3 * sizeof( float ) ) );
		}
		glBindVertexArray( quadVAO );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		glBindVertexArray( 0 );
	}


	OpenGLEnvironmentMap::OpenGLEnvironmentMap( const SharedPtr<Texture>& a_EquirectangularTexture )
	{
		const uint32_t CubeMapSize = 1024;
		TextureSpecification cubeMapSpecification =
		{
			.Width = CubeMapSize,
			.Height = CubeMapSize,
			.Depth = 0u,
			.TextureFormat = ETextureFormat::RGB16F,
			.GenerateMips = false,
			.MinFilter = ETextureFilter::Linear,
			.MagFilter = ETextureFilter::Linear,
			.WrapS = ETextureWrap::ClampToEdge,
			.WrapT = ETextureWrap::ClampToEdge,
			.WrapR = ETextureWrap::ClampToEdge,
		};

		SharedPtr<CubeMap> environmentCubeMap( CubeMap::Create( cubeMapSpecification, a_EquirectangularTexture ) );

		// Diffuse convolution
		{
			TextureSpecification irradianceMapSpecification =
			{
				.Width = 32,
				.Height = 32,
				.Depth = 0u,
				.TextureFormat = ETextureFormat::RGB16F,
				.GenerateMips = false,
				.MinFilter = ETextureFilter::Linear,
				.MagFilter = ETextureFilter::Linear,
				.WrapS = ETextureWrap::ClampToEdge,
				.WrapT = ETextureWrap::ClampToEdge,
				.WrapR = ETextureWrap::ClampToEdge,
			};

			m_IrradianceMap.reset( CubeMap::Create( irradianceMapSpecification ) );
			PerformDiffuseConvolution( environmentCubeMap );
		}

		// Specular convolution
		{
			TextureSpecification radianceMapSpecification =
			{
				.Width = CubeMapSize,
				.Height = CubeMapSize,
				.Depth = 0u,
				.TextureFormat = ETextureFormat::RGB16F,
				.GenerateMips = true,
				.MinFilter = ETextureFilter::LinearMipmapLinear,
				.MagFilter = ETextureFilter::Linear,
				.WrapS = ETextureWrap::ClampToEdge,
				.WrapT = ETextureWrap::ClampToEdge,
				.WrapR = ETextureWrap::ClampToEdge,
			};

			m_RadianceMap.reset( CubeMap::Create( radianceMapSpecification ) );
			PerformSpecularConvolution( environmentCubeMap );
		}
	}

	void OpenGLEnvironmentMap::PerformDiffuseConvolution( SharedPtr<CubeMap> a_EnvironmentCubeMap )
	{
		m_IrradianceMap->Bind();

		FramebufferSpecification framebufferSpecification =
		{
			.Width = m_IrradianceMap->GetWidth(),
			.Height = m_IrradianceMap->GetHeight(),
			.Attachments = { EFramebufferTextureFormat::RGBA16F },
		};
		SharedPtr<Framebuffer> framebuffer = Framebuffer::Create( framebufferSpecification );
		SharedPtr<RenderBuffer> renderBuffer = RenderBuffer::Create( m_IrradianceMap->GetWidth(), m_IrradianceMap->GetHeight(), EFramebufferTextureFormat::DEPTH24STENCIL8 );

		// Bind and configure framebuffer
		framebuffer->Bind();
		renderBuffer->Bind();

		RenderCommand::SetCullMode( false );
		UniquePtr<Shader> irradianceShader( Shader::Create() );
		irradianceShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/EnvironmentMap/IrradianceConvolution.glsl" );

		irradianceShader->Bind();
		{
			irradianceShader->SetMatrix4( "u_Projection", s_CaptureProjection );

			// Bind environment cube map to texture unit 0
			a_EnvironmentCubeMap->Bind();
			irradianceShader->SetInt( "u_EnvironmentMap", 0 );

			RenderCommand::SetViewport( 0, 0, m_IrradianceMap->GetWidth(), m_IrradianceMap->GetHeight() );

			// Iterate through cube faces
			for ( uint32_t i = 0; i < 6; i++ )
			{
				irradianceShader->SetMatrix4( "u_View", s_CaptureVies[i] );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_IrradianceMap->GetRendererID(), 0 );

				RenderCommand::SetClearColor( { 1.0f, 0.0f, 0.0f, 1.0f } ); // For debugging, change to a known color
				RenderCommand::Clear();
				RenderCube();
			}

			framebuffer->Unbind(); // Unbind the framebuffer
		}

		RenderCommand::SetCullMode( true );

		m_IrradianceMap->Unbind(); // Unbind the irradiance map
		irradianceShader->Unbind(); // Unbind the shader
	}

	void OpenGLEnvironmentMap::PerformSpecularConvolution( SharedPtr<CubeMap> a_EnvironmentCubeMap )
	{
		m_RadianceMap->Bind();

		FramebufferSpecification framebufferSpecification =
		{
			.Width = m_RadianceMap->GetWidth(),
			.Height = m_RadianceMap->GetHeight(),
			.Attachments = { EFramebufferTextureFormat::RGBA16F },
		};
		SharedPtr<Framebuffer> framebuffer = Framebuffer::Create( framebufferSpecification );
		SharedPtr<RenderBuffer> renderBuffer = RenderBuffer::Create( m_RadianceMap->GetWidth(), m_RadianceMap->GetHeight(), EFramebufferTextureFormat::DEPTH24STENCIL8 );

		// Bind and configure framebuffer
		framebuffer->Bind();
		renderBuffer->Bind();

		UniquePtr<Shader> prefilterShader( Shader::Create() );
		prefilterShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/EnvironmentMap/PreFilter-EnvironmentMap.glsl" );

		RenderCommand::SetCullMode( false );
		prefilterShader->Bind();
		{
			prefilterShader->SetMatrix4( "u_Projection", s_CaptureProjection );
			prefilterShader->SetInt( "u_EnvironmentMap", 0 );
			a_EnvironmentCubeMap->Bind();

			const uint32_t MaxMipLevels = 5;
			// For each mip level
			for ( uint32_t mip = 0; mip < MaxMipLevels; ++mip )
			{
				uint32_t mipWidth = framebufferSpecification.Width * std::pow( 0.5, mip );
				uint32_t mipHeight = framebufferSpecification.Height * std::pow( 0.5, mip );

				renderBuffer->Bind();
				renderBuffer->Resize( mipWidth, mipHeight );
				RenderCommand::SetViewport( 0, 0, mipWidth, mipHeight );

				float roughness = (float)mip / (float)( MaxMipLevels - 1 );
				prefilterShader->SetFloat( "u_Roughness", roughness );

				// For each cube face
				for ( uint32_t i = 0; i < 6; i++ )
				{
					prefilterShader->SetMatrix4( "u_View", s_CaptureVies[i] );
					glFramebufferTexture2D( GL_FRAMEBUFFER,
						GL_COLOR_ATTACHMENT0,
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						m_RadianceMap->GetRendererID(), mip );

					RenderCommand::SetClearColor( { 1.0f, 0.0f, 0.0f, 1.0f } ); // For debugging, change to a known color
					RenderCommand::Clear();
					RenderCube();
				}
			}
			framebuffer->Unbind(); // Unbind the framebuffer
			a_EnvironmentCubeMap->Unbind(); // Unbind the irradiance map
		}
		prefilterShader->Unbind(); // Unbind the shader
		RenderCommand::SetCullMode( true );
	}

}
