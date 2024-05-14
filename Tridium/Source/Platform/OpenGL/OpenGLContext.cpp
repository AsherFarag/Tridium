#include "tripch.h"
#include "OpenGLContext.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace Tridium {
	OpenGLContext::OpenGLContext( GLFWwindow* a_Window )
		: m_Window(a_Window)
	{
	}

	bool OpenGLContext::Init()
	{
		// - Glad Initialization -
		int status = gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress );
		TE_CORE_ASSERT( status, "Failed to initialize glad!" );

		return status;
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers( m_Window );
	}

}


