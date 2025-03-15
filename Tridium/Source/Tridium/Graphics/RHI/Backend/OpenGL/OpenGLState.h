#pragma once
#include "OpenGLPipelineState.h"
#include "OpenGLShaderBindingLayout.h"

namespace Tridium {

	namespace OpenGLState {

		extern WeakPtr<OpenGLGraphicsPipelineState> s_BoundGraphicsPSO;
		extern WeakPtr<OpenGLShaderBindingLayout> s_BoundSBL;
		extern GLuint s_BoundIBO;
		extern GLuint s_BoundVBO;
		extern GLuint s_BoundVAO;
		extern GLenum s_BoundPrimitiveTopology;

	}
}