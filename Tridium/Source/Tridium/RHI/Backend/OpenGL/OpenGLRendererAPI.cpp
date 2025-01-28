#include "tripch.h"
#include "OpenGLRendererAPI.h"
#include "OpenGL.h"

namespace Tridium {

	bool OpenGLRendererAPI::Init( const RHIConfig& a_Config )
	{
		int status = gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress );
		return status;
	}

	bool OpenGLRendererAPI::Shutdown()
	{
		return true;
	}

	bool OpenGLRendererAPI::Present()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		if ( GLFWwindow* window = glfwGetCurrentContext() )
		{
			glfwSwapBuffers( window );
			return true;
		}

		return false;
	}

} // namespace Tridium