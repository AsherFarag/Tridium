#pragma once
#include "Tridium/Graphics/Rendering/RenderingContext.h"

struct GLFWwindow;

namespace Tridium {

	class OpenGLContext : public RenderingContext
	{
	public:
		OpenGLContext( GLFWwindow* a_Window );

		virtual bool Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_Window;
	};

}


