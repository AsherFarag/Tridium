#pragma once
#include "Tridium/Rendering/RenderingAPI.h"

namespace Tridium {

	class OpenGLRenderingAPI : public RendererAPI
	{
		virtual void Init() override;
		virtual void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) override;
		virtual void SetDepthCompare( EDepthCompareOperator a_DepthCompareOperator ) override;
		virtual void SetCullMode( bool enabled ) override;
		virtual void SetClearColor( const Vector4& color ) override;
		virtual void Clear() override;

		virtual void DrawIndexed( const SharedPtr<VertexArray>& a_VertexArray ) override;
	};

}


