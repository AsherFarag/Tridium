#include "tripch.h"
#include "OpenGLState.h"
#include "OpenGLPipelineState.h"
#include "OpenGLShaderBindings.h"

namespace Tridium::GLState {

	WeakPtr<RHIGraphicsPipelineState_OpenGLImpl> s_BoundGraphicsPSO;
	WeakPtr<RHIBindingLayout_OpenGLImpl> s_BoundSBL;
	GLenum s_BoundPrimitiveTopology = GL_TRIANGLES;
	GLuint s_BoundProgram = 0u;
	GLuint s_BoundFBO = 0u;
	GLuint s_BoundIBO = 0u;
	GLuint s_BoundVBO = 0u;
	GLuint s_BoundVAO = 0u;
	GLuint s_FBO = 0u;
	UnorderedMap<hash_t, GLuint> s_CachedVAOs;

	void ClearState()
	{
		OpenGL3::DeleteFramebuffers( 1, &s_FBO );
		s_FBO = 0;
		s_BoundFBO = 0;

		for ( auto& [hash, vao] : s_CachedVAOs )
		{
			OpenGL3::DeleteVertexArrays( 1, &vao );
		}
		s_CachedVAOs.clear();
	}

}