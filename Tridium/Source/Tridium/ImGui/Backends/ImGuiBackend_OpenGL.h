#pragma once
#include <Tridium/ImGui/ImGuiBackend.h>
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Graphics/RHI/Backend/OpenGL/RHI_OpenGLImpl.h>
#if 0
//#include <backends/imgui_impl_opengl3.cpp>

namespace Tridium::OpenGL {

	class RendererImGuiInterface_OpenGL : public IRendererImGuiInterface
	{
	public:
		bool Init( IDynamicRHI* a_RHI ) override
		{
			if ( !ImGui_ImplOpenGL3_Init( "#version 410" ) )
			{
				ENSURE( false, "Failed to initialize ImGui OpenGL backend!" );
				return false;
			}
			return true;
		}

		void Shutdown() override
		{
			ImGui_ImplOpenGL3_Shutdown();
		}

		void NewFrame() override
		{
			ImGui_ImplOpenGL3_NewFrame();
		}

		void RenderDrawData( ImDrawData* a_DrawData, const RHICommandListRef& a_CmdList, const RHITextureRef& a_RenderTarget ) override
		{
			ASSERT( a_RenderTarget != nullptr, "Render target is null!" );

			// Bind the the render target
			GLFramebufferWrapper framebuffer{ true };
			OpenGL3::BindFramebuffer( GL_FRAMEBUFFER, framebuffer );

			OpenGL3::FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
										   GL_TEXTURE_2D, *a_RenderTarget->NativePtrAs<GLuint>(), 0);

			OpenGL2::DrawBuffer( GL_COLOR_ATTACHMENT0 );

			OpenGL3::ClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
			OpenGL3::Clear( GL_COLOR_BUFFER_BIT );

			ImGui_ImplOpenGL3_RenderDrawData( a_DrawData );
		}

	};

} // namespace Tridium

#endif