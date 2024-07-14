#pragma once

#include "RenderingAPI.h"

namespace Tridium {

	class Camera;
	class Material;

	class Renderer
	{
	public:
		static void BeginScene( const Camera& a_Camera, const Matrix4& a_ViewMatrix );
		static void EndScene();

		static void Submit( const Ref<Shader>& a_Shader, const Ref<VertexArray>& a_VertexArray, const Matrix4& a_Transform );

		static inline auto GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			Matrix4 ViewProjectionMatrix;
		};
		static SceneData* m_SceneData;
	};

}