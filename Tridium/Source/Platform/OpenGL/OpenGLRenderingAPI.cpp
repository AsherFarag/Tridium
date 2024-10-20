#include "tripch.h"
#include "OpenGLRenderingAPI.h"

#include "glad/glad.h"

namespace Tridium {

	namespace Util {

		static GLenum DepthCompareOperatorToOpenGL( EDepthCompareOperator a_DepthCompareOperator )
		{
			switch ( a_DepthCompareOperator )
			{
			case EDepthCompareOperator::None: return GL_NONE;
			case EDepthCompareOperator::Never: return GL_NEVER;
			case EDepthCompareOperator::NotEqual: return GL_NOTEQUAL;
			case EDepthCompareOperator::Less: return GL_LESS;
			case EDepthCompareOperator::LessOrEqual: return GL_LEQUAL;
			case EDepthCompareOperator::Greater: return GL_GREATER;
			case EDepthCompareOperator::GreaterOrEqual: return GL_GEQUAL;
			case EDepthCompareOperator::Equal: return GL_EQUAL;
			case EDepthCompareOperator::Always: return GL_ALWAYS;
			}

			TE_CORE_ASSERT( false, "Unknown depth compare operator!" );
			return 0;
		}
	}


	void OpenGLRenderingAPI::Init()
	{
		glEnable( GL_DEPTH_TEST );
		TODO( "Set up functions to enable these!" );
		glEnable( GL_MULTISAMPLE );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	void OpenGLRenderingAPI::SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
	{
		glViewport( x, y, width, height );
	}

	void OpenGLRenderingAPI::SetDepthCompare( EDepthCompareOperator a_DepthCompareOperator )
	{
		glDepthFunc( Util::DepthCompareOperatorToOpenGL( a_DepthCompareOperator ) );
	}

	void OpenGLRenderingAPI::SetCullMode( bool enabled )
	{
		if ( enabled )
			glEnable( GL_CULL_FACE );
		else
			glDisable( GL_CULL_FACE );
	}

	void OpenGLRenderingAPI::SetClearColor( const Vector4& color )
	{
		glClearColor( color.r, color.g, color.b, color.a );
	}

	void OpenGLRenderingAPI::Clear()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void OpenGLRenderingAPI::DrawIndexed( const SharedPtr<VertexArray>& a_VertexArray )
	{
		glDrawElements( GL_TRIANGLES, a_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr );
	}

}