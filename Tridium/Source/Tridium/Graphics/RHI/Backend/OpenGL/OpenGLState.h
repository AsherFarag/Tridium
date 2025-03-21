#pragma once
#include "OpenGL4.h"

namespace Tridium {

	class OpenGLGraphicsPipelineState;
	class OpenGLShaderBindingLayout;

	namespace GLState {

		extern WeakPtr<OpenGLGraphicsPipelineState> s_BoundGraphicsPSO;
		extern WeakPtr<OpenGLShaderBindingLayout> s_BoundSBL;
		extern GLuint s_BoundProgram;
		extern GLuint s_BoundFBO;
		extern GLuint s_BoundIBO;
		extern GLuint s_BoundVBO;
		extern GLuint s_BoundVAO;
		extern GLenum s_BoundPrimitiveTopology;

		// Used for constructing Framebuffers on the fly
		extern GLuint s_FBO;
		
		// Cache of VAOs that are registered via the hash of their VBO and IBO
		extern UnorderedMap<hash_t, GLuint> s_CachedVAOs;

		inline GLuint GetOrEmplaceCachedVAO( GLuint a_VBO, GLuint a_IBO = 0 )
		{
			hash_t vaoHash = 0;
			vaoHash = Hashing::HashCombine( vaoHash, a_VBO );
			vaoHash = Hashing::HashCombine( vaoHash, a_IBO );
			auto it = s_CachedVAOs.find( vaoHash );
			if ( it != s_CachedVAOs.end() )
			{
				return it->second;
			}

			GLuint vao = 0;
			OpenGL3::GenVertexArrays( 1, &vao );
			s_CachedVAOs.insert( { vaoHash, vao } );
			return vao;
		}

		inline void BindProgram( GLuint a_Program )
		{
			if ( s_BoundProgram != a_Program )
			{
				s_BoundProgram = a_Program;
				OpenGL2::UseProgram( a_Program );
			}
		}

		inline void BindFBO( GLuint a_FBO )
		{
			if ( s_BoundFBO != a_FBO )
			{
				s_BoundFBO = a_FBO;
				OpenGL3::BindFramebuffer( GL_FRAMEBUFFER, a_FBO );
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

		// Resets all values and deletes any OpenGL objects stored in the state
		void ClearState();
	}
}