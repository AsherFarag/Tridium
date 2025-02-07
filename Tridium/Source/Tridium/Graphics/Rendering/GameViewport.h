#pragma once

namespace Tridium {

	class Shader;
	class VertexArray;
	class Framebuffer;

	class GameViewport
	{
	public:
		GameViewport();

		void Init( uint32_t a_Width, uint32_t a_Height );
		void RenderToWindow();
		void Resize( uint32_t a_Width, uint32_t a_Height );

		SharedPtr<Framebuffer> GetFramebuffer() { return m_FBO; }

	private:
		uint32_t m_Width, m_Height;
		SharedPtr<Framebuffer> m_FBO;
		SharedPtr<Shader> m_Shader;
		SharedPtr<VertexArray> m_QuadVAO;
	};

}