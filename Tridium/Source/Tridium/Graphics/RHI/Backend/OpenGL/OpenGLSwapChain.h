#pragma once
#include "OpenGLCommon.h"
#include "OpenGLTexture.h"

namespace Tridium {

	struct OpenGLFramebuffer
	{
		RHITextureRef BackBufferTexture = nullptr;
		GLuint ShaderID = 0;
		struct Quad
		{

			static constexpr float Vertices[] =
			{
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f
			};

			static constexpr uint32_t Indices[] = { 0, 1, 2, 2, 3, 0 };

			GLuint VAO = 0;
			GLuint VBO = 0;
			GLuint IBO = 0;

			~Quad()
			{
				if ( VAO )
				{
					OpenGL3::DeleteVertexArrays( 1, &VAO );
				}
				if ( VBO )
				{
					OpenGL3::DeleteBuffers( 1, &VBO );
				}
				if ( IBO )
				{
					OpenGL3::DeleteBuffers( 1, &IBO );
				}
			}

		} ScreenQuad;

		bool Init( const RHISwapChainDescriptor& a_Desc );
		void Resize( uint32_t a_Width, uint32_t a_Height );

		~OpenGLFramebuffer()
		{
			if ( ShaderID )
			{
				OpenGL3::DeleteProgram( ShaderID );
			}
		}
	};

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHISwapChain_OpenGLImpl, RHISwapChain )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHISwapChain_OpenGLImpl, ERHInterfaceType::OpenGL );
		RHISwapChain_OpenGLImpl( const DescriptorType& a_Desc );
		bool Release() override;
		bool IsValid() const override { return Window != nullptr; }
		const void* NativePtr() const override { return Window; }
		bool Present() override;
		RHITextureRef GetBackBuffer() override;
		bool Resize( uint32_t a_Width, uint32_t a_Height ) override;
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		const auto& GetFramebuffer() const { return m_Framebuffer; }

		GLFWwindow* Window = nullptr;

	private:
		OpenGLFramebuffer m_Framebuffer;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};

} // namespace Tridium