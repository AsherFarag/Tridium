#include "tripch.h"
#include "OpenGLRenderingAPI.h"
#include <Tridium/Graphics/Rendering/Mesh.h>

#include "glad/glad.h"
#include "OpenGLRenderingAPI.h"

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

			ASSERT_LOG( false, "Unknown depth compare operator!" );
			return 0;
		}
	}


	void OpenGLRenderingAPI::Init()
	{
		glEnable( GL_DEPTH_TEST );
		glDepthMask( GL_TRUE );
		TODO( "Set up functions to enable these!" );
		glEnable( GL_MULTISAMPLE );
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
	}

	void OpenGLRenderingAPI::SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
	{
		glViewport( x, y, width, height );
	}

	void OpenGLRenderingAPI::SetBlendmode( EBlendMode a_BlendMode )
	{
		switch ( a_BlendMode )
		{
		case EBlendMode::None:
			glDisable( GL_BLEND );
			break;
		case EBlendMode::Alpha:
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			break;
		case EBlendMode::Additive:
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE );
			break;
		default:
			ASSERT_LOG( false, "Unknown blend mode!" );
			break;
		}
	}

	void OpenGLRenderingAPI::SetDepthTest( bool enabled )
	{
		if ( enabled )
			glEnable( GL_DEPTH_TEST );
		else
			glDisable( GL_DEPTH_TEST );
	}

	void OpenGLRenderingAPI::SetDepthCompare( EDepthCompareOperator a_DepthCompareOperator )
	{
		glDepthFunc( Util::DepthCompareOperatorToOpenGL( a_DepthCompareOperator ) );
	}

	void OpenGLRenderingAPI::SetCullMode( ECullMode a_CullMode )
	{
		switch ( a_CullMode )
		{
		case ECullMode::None:
		{
			glDisable( GL_CULL_FACE );
			break;
		}
		case ECullMode::Front:
		{
			glEnable( GL_CULL_FACE );
			glCullFace( GL_FRONT );
			break;
		}
		case ECullMode::Back:
		{
			glEnable( GL_CULL_FACE );
			glCullFace( GL_BACK );
			break;
		}
		default:
			ASSERT_LOG( false, "Unknown cull mode!" );
		}
	}

	void OpenGLRenderingAPI::SetClearColor( const Vector4& color )
	{
		glClearColor( color.r, color.g, color.b, color.a );
	}

	void OpenGLRenderingAPI::SetLineWidth( float width )
	{
		glLineWidth( width );
	}

	void OpenGLRenderingAPI::SetPolygonMode( EFaces a_Faces, EPolygonMode a_PolygonMode )
	{
		switch ( a_PolygonMode )
		{
		case EPolygonMode::Fill:
			glPolygonMode( a_Faces == EFaces::Front ? GL_FRONT : a_Faces == EFaces::Back ? GL_BACK : GL_FRONT_AND_BACK, GL_FILL );
			break;
		case EPolygonMode::Line:
			glPolygonMode( a_Faces == EFaces::Front ? GL_FRONT : a_Faces == EFaces::Back ? GL_BACK : GL_FRONT_AND_BACK, GL_LINE );
			break;
		case EPolygonMode::Point:
			glPolygonMode( a_Faces == EFaces::Front ? GL_FRONT : a_Faces == EFaces::Back ? GL_BACK : GL_FRONT_AND_BACK, GL_POINT );
			break;
		default:
			ASSERT_LOG( false, "Unknown polygon mode!" );
			break;
		}
	}

	void OpenGLRenderingAPI::Clear()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void OpenGLRenderingAPI::DrawIndexed( const SharedPtr<VertexArray>& a_VertexArray )
	{
		glDrawElements( GL_TRIANGLES, a_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr );
	}

	void OpenGLRenderingAPI::DrawInstanced( const SharedPtr<VertexArray>& a_VertexArray, uint32_t a_InstanceCount )
	{
		glDrawElementsInstanced( GL_TRIANGLES, a_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr, a_InstanceCount );
	}

}