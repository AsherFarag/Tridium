#pragma once
#include "OpenGL4.h"

namespace Tridium {

	class OpenGLGraphicsPipelineState;
	class OpenGLShaderBindingLayout;

	namespace OpenGLState {

		extern WeakPtr<OpenGLGraphicsPipelineState> s_BoundGraphicsPSO;
		extern WeakPtr<OpenGLShaderBindingLayout> s_BoundSBL;
		extern GLuint s_BoundProgram;
		extern GLuint s_BoundIBO;
		extern GLuint s_BoundVBO;
		extern GLuint s_BoundVAO;
		extern GLenum s_BoundPrimitiveTopology;

		inline void BindProgram( GLuint a_Program )
		{
			if ( s_BoundProgram != a_Program )
			{
				s_BoundProgram = a_Program;
				OpenGL2::UseProgram( a_Program );
			}
		}

		inline void BindIndexBuffer( GLuint a_IBO )
		{
			if ( s_BoundIBO != a_IBO )
			{
				s_BoundIBO = a_IBO;
				OpenGL1::BindBuffer( GL_ELEMENT_ARRAY_BUFFER, a_IBO );
			}
		}

		inline void BindVertexBuffer( GLuint a_VBO )
		{
			if ( s_BoundVBO != a_VBO )
			{
				s_BoundVBO = a_VBO;
				OpenGL1::BindBuffer( GL_ARRAY_BUFFER, a_VBO );
			}
		}

		inline void BindVertexArray( GLuint a_VAO )
		{
			if ( s_BoundVAO != a_VAO )
			{
				s_BoundVAO = a_VAO;
				OpenGL3::BindVertexArray( a_VAO );
			}
		}

	}
}