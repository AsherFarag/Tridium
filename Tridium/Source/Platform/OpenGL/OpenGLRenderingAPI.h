#pragma once
#include "Tridium/Rendering/RenderingAPI.h"

namespace Tridium {

	class OpenGLRenderingAPI : public RendererAPI
	{
		virtual void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) override;

		virtual void SetClearColor( const Vector4& color ) override;
		virtual void Clear() override;

		virtual void DrawIndexed( const Ref<VertexArray>& a_VertexArray ) override;
	};

}


