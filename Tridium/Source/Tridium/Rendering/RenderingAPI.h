#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Math/Math.h>
#include <Tridium/Rendering/Pipeline.h>
#include "Buffer.h"
#include "VertexArray.h"

namespace Tridium {



	class RendererAPI
	{
	public:
		enum class API { None = 0, OpenGL };

	public:
		virtual void Init() {}
		virtual void SetViewport( uint32_t a_X, uint32_t a_Y, uint32_t a_Width, uint32_t a_Height ) = 0;

		virtual void SetDepthTest( bool a_Enabled ) = 0;
		virtual void SetDepthCompare( EDepthCompareOperator a_DepthCompareOperator ) = 0;
		virtual void SetCullMode( ECullMode a_CullMode ) = 0;
		virtual void SetClearColor( const Vector4& a_Color ) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed( const SharedPtr<VertexArray>& a_VertexArray ) = 0;

		static inline const API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}