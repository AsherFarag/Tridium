#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Math/Math.h>

#include "Buffer.h"
#include "VertexArray.h"

namespace Tridium {

	class RendererAPI
	{
	public:
		enum class API { None = 0, OpenGL };

	public:
		virtual void Init() {}
		virtual void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) = 0;

		virtual void SetClearColor( const Vector4& color ) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed( const Ref<VertexArray>& a_VertexArray ) = 0;

		static inline const API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}