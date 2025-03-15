#include "tripch.h"
#include "OpenGLState.h"

namespace Tridium::OpenGLState {

	WeakPtr<OpenGLGraphicsPipelineState> s_BoundGraphicsPSO;
	WeakPtr<OpenGLShaderBindingLayout> s_BoundSBL;
	GLenum s_BoundPrimitiveTopology = GL_TRIANGLES;
	GLuint s_BoundIBO = 0u;
	GLuint s_BoundVBO = 0u;
	GLuint s_BoundVAO = 0u;

}