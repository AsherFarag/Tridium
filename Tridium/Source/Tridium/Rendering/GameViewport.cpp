#include "tripch.h"
#include "GameViewport.h"
#include "Shader.h"
#include "VertexArray.h"
#include "RenderCommand.h"
#include "Framebuffer.h"

namespace Tridium {

	GameViewport::GameViewport()
		: m_Width( 1280 ), m_Height( 720 )
	{
	}

	void GameViewport::Init( uint32_t a_Width, uint32_t a_Height )
	{
		m_Width = a_Width;
		m_Height = a_Height;

		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { EFramebufferTextureFormat::RGBA16F, EFramebufferTextureFormat::Depth };
		FBOspecification.Width = m_Width;
		FBOspecification.Height = m_Height;
		m_FBO = Framebuffer::Create( FBOspecification );

		// Create the shader
		{
			std::string vertexSrc = R"(
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

			std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}
			)";

			m_Shader.reset( Shader::Create( vertexSrc, fragmentSrc ) );
		}

		// Create Quad VAO
		{
			BufferLayout layout =
			{
				{ EShaderDataType::Float3, "a_Position" },
				{ EShaderDataType::Float2, "a_UV" }
			};

			float vertices[] =
			{
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f
			};

			uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

			m_QuadVAO = VertexArray::Create();
			SharedPtr<VertexBuffer> vertexBuffer = VertexBuffer::Create( vertices, sizeof( vertices ) );
			vertexBuffer->SetLayout( layout );
			m_QuadVAO->AddVertexBuffer( vertexBuffer );
			SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
			m_QuadVAO->SetIndexBuffer( indexBuffer );
		}
	}

	void GameViewport::RenderToWindow()
	{
		// Render the framebuffer to the window
		m_FBO->Unbind();
		RenderCommand::SetViewport( 0, 0, m_Width, m_Height );
		RenderCommand::Clear();

		m_Shader->Bind();
		m_FBO->BindAttatchment( 0 );

		m_QuadVAO->Bind();
		RenderCommand::DrawIndexed( m_QuadVAO );
		m_QuadVAO->Unbind();

		m_FBO->Unbind();
		m_Shader->Unbind();
	}

	void GameViewport::Resize( uint32_t a_Width, uint32_t a_Height )
	{
		if ( m_Width != a_Width || m_Height != a_Height )
		{
			m_Width = a_Width;
			m_Height = a_Height;
			m_FBO->Resize( m_Width, a_Height );
		}
	}
}
