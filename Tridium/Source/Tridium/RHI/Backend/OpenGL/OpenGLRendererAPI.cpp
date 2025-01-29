#include "tripch.h"
#include "OpenGLRendererAPI.h"
#include "OpenGL.h"
#include <iostream>

namespace Tridium::GL {

#if RHI_DEBUG_ENABLED

	static Array<String> s_OpenGLDebugMessages;

	void APIENTRY DebugCallback(
		GLenum a_Source, GLenum a_Type, GLuint a_ID,
		GLenum a_Severity,
		GLsizei a_Length, const GLchar* a_Message, const void* a_UserParam )
	{
		switch ( a_Severity )
		{
		case GL_DEBUG_SEVERITY_HIGH:
			s_OpenGLDebugMessages.EmplaceBack( "Error: " + String( a_Message ) );
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			s_OpenGLDebugMessages.EmplaceBack( "Warning: " + String( a_Message ) );
			break;
		case GL_DEBUG_SEVERITY_LOW:
			s_OpenGLDebugMessages.EmplaceBack( "Info: " + String( a_Message ) );
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			s_OpenGLDebugMessages.EmplaceBack( "Debug: " + String( a_Message ) );
			break;
		}
	}

	void OpenGLRendererAPI::DumpDebug()
	{
		std::cout << "OpenGL Debug Dump:\n";
		for ( const String& message : s_OpenGLDebugMessages )
		{
			std::cout << '	' << message << '\n';
		}
		std::cout << std::endl;

		s_OpenGLDebugMessages.Clear();

	}

#endif // RHI_DEBUG_ENABLED

	bool OpenGLRendererAPI::Init( const RHIConfig& a_Config )
	{
	#if RHI_DEBUG_ENABLED
		if ( a_Config.UseDebug )
		{
			// Enable OpenGL debug output
			glEnable( GL_DEBUG_OUTPUT );
			glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
			glDebugMessageCallback( DebugCallback, nullptr );
			glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
		}
	#endif

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