#pragma once
#include "OpenGLCommon.h"
#include "OpenGLTexture.h"

namespace Tridium {

	struct OpenGLFramebuffer
	{
		RHITextureRef BackBufferTexture = nullptr;
		GLuint FramebufferID = 0;
		GLuint ShaderID = 0;
		struct Quad
		{
			static constexpr float Vertices[] = {
				// Positions   // TexCoords
				-1.0f,  1.0f,  0.0f, 1.0f,
				-1.0f, -1.0f,  0.0f, 0.0f,
				 1.0f, -1.0f,  1.0f, 0.0f,

				-1.0f,  1.0f,  0.0f, 1.0f,
				 1.0f, -1.0f,  1.0f, 0.0f,
				 1.0f,  1.0f,  1.0f, 1.0f
			};

			GLuint VAO = 0;
			GLuint VBO = 0;

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
			}

		} ScreenQuad;

		bool Init( const RHISwapChainDescriptor& a_Desc );

		~OpenGLFramebuffer()
		{
			if ( FramebufferID )
			{
				OpenGL3::DeleteFramebuffers( 1, &FramebufferID );
			}

			if ( ShaderID )
			{
				OpenGL3::DeleteProgram( ShaderID );
			}
		}
	};

	class OpenGLSwapChain : public RHISwapChain
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( OpenGL );
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override { return Window != nullptr; }
		const void* NativePtr() const override { return Window; }
		bool Present() override;
		RHITextureRef GetBackBuffer() override;
		bool Resize( uint32_t a_Width, uint32_t a_Height ) override;

		const auto& GetFramebuffer() const { return m_Framebuffer; }

		GLFWwindow* Window = nullptr;

	private:
		OpenGLFramebuffer m_Framebuffer;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};

} // namespace Tridium