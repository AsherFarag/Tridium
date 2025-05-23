#pragma once
#include "RenderingAPI.h"
#include "Pipeline.h"

namespace Tridium {

	class RenderCommand
	{
	public:
		static inline void Init() { s_RendererAPI->Init(); }

		static inline void SetViewport( uint32_t a_X, uint32_t a_Y, uint32_t a_Width, uint32_t a_Height ) { s_RendererAPI->SetViewport( a_X, a_Y, a_Width, a_Height ); }

		static inline void SetBlendmode( EBlendMode a_BlendMode ) { s_RendererAPI->SetBlendmode( a_BlendMode ); }

		static inline void SetDepthTest( bool a_Enabled ) { s_RendererAPI->SetDepthTest( a_Enabled ); }

		static inline void SetDepthCompare( EDepthCompareOperator a_DepthCompareOperator ) { s_RendererAPI->SetDepthCompare( a_DepthCompareOperator ); }

		static inline void SetCullMode( ECullMode a_CullMode ) { s_RendererAPI->SetCullMode( a_CullMode ); }

		static inline void SetClearColor( const Vector4& a_Color ) { s_RendererAPI->SetClearColor( a_Color ); }

		static inline void SetLineWidth( float a_Width ) { s_RendererAPI->SetLineWidth( a_Width ); }

		static inline void SetPolygonMode( EFaces a_Faces, EPolygonMode a_PolygonMode ) { s_RendererAPI->SetPolygonMode( a_Faces, a_PolygonMode ); }

		static inline void Clear() { s_RendererAPI->Clear(); }

		static inline void DrawIndexed( const SharedPtr<VertexArray>& a_VertexArray ) { s_RendererAPI->DrawIndexed( a_VertexArray ); }

	private:
		static UniquePtr<RendererAPI> s_RendererAPI;
	};

}