#pragma once

#include "RenderingAPI.h"

namespace Tridium {

	class CameraComponent;
	class TransformComponent;

	class Renderer
	{
	public:
		static void BeginScene( const CameraComponent& a_Camera, const TransformComponent& a_CameraTransform );
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