#include "tripch.h"
#include "OpenGLState.h"

namespace Tridium::OpenGLState {

	RHIShaderBindingLayoutWeakRef s_BoundSBL{};
	GLuint s_BoundIBO = 0u;
	GLuint s_BoundVBO = 0u;
	GLuint s_BoundVAO = 0u;

}