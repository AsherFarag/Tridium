#pragma once
#include "Tridium/Graphics/Rendering/RenderingAPI.h"

namespace Tridium {

	class OpenGLRenderingAPI : public RendererAPI
	{
		virtual void Init() override;
		virtual void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) override;
		virtual void SetBlendmode( EBlendMode a_BlendMode ) override;
		virtual void SetDepthTest( bool enabled ) override;
		virtual void SetDepthCompare( EDepthCompareOperator a_DepthCompareOperator ) override;
		virtual void SetCullMode( ECullMode a_CullMode ) override;
		virtual void SetClearColor( const Vector4& color ) override;
		virtual void SetLineWidth( float width ) override;
		virtual void SetPolygonMode( EFaces a_Faces, EPolygonMode a_PolygonMode ) override;
		virtual void Clear() override;

		virtual void DrawIndexed( const SharedPtr<VertexArray>& a_VertexArray ) override;
		virtual void DrawInstanced( const SharedPtr<VertexArray>& a_VertexArray, uint32_t a_InstanceCount ) override;
	};

}


