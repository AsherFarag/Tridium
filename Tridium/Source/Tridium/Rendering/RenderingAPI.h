#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Math/Math.h>

namespace Tridium {

	class RenderingAPI
	{
	public:
		virtual void Init() {}
		virtual void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) {}

		virtual void SetClearColor( const Vector4& color ) {}
		virtual void Clear() {}

		//virtual void DrawIndexed( const SharedPtr<VertexArray>& vertexArray, uint32_t indexCount = 0 ) {}
		//virtual void DrawLines( const SharedPtr<VertexArray>& vertexArray, uint32_t vertexCount ) {}

		virtual void SetLineWidth( float width ) {}
	};

}