#pragma once
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include <Tridium/Scene/Scene.h>

namespace Tridium {

	extern Vector3 s_AmbientColor;
	extern Vector3 s_LightDirection;
	extern Vector3 s_LightColor;
	extern float s_LightIntensity;

	// Forward declarations
	class VertexArray;

	struct DrawCall
	{
		SharedPtr<VertexArray> VAO;
		AssetHandle Material;
		Matrix4 Transform;
	};

	class SceneRenderer
	{
	public:
		SceneRenderer( const SharedPtr<Scene>& a_Scene );

		void Render( const Camera& a_Camera, const Matrix4& a_View );

	protected:
		void BeginScene( const Camera& a_Camera, const Matrix4& a_View );
		void EndScene();

		void Flush();

		void SubmitDrawCall( const DrawCall& a_DrawCall );
		void PerformDrawCall( const DrawCall& a_DrawCall );

	private:
		SharedPtr<Scene> m_Scene;
		std::vector<DrawCall> m_DrawCalls;
		Matrix4 m_ViewProjectionMatrix;

		// TEMP ?
		SharedPtr<Shader> m_DefaultShader;
		SharedPtr<Material> m_DefaultMaterial;
	};

}