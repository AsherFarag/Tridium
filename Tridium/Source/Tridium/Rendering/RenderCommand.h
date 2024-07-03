#pragma once

#include "RenderingAPI.h"

namespace Tridium {

	class RenderCommand
	{
	public:
		static inline void Init()
		{
			s_RendererAPI->Init();
		}

		static inline void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) {
			s_RendererAPI->SetViewport( x, y, width, height );
		}

		static inline void SetCullMode( bool enabled ) {
			s_RendererAPI->SetCullMode( enabled );
		}

		static inline void SetClearColor( const Vector4& color ) {
			s_RendererAPI->SetClearColor( color );
		}

		static inline void Clear() {
			s_RendererAPI->Clear();
		}

		static inline void DrawIndexed( const Ref<VertexArray>& a_VertexArray )
		{
			s_RendererAPI->DrawIndexed( a_VertexArray );
		}

	private:
		static UniquePtr<RendererAPI> s_RendererAPI;
	};

}