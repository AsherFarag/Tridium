#pragma once
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include <Tridium/Scene/Scene.h>
#include <Tridium/Rendering/Lights.h>

namespace Tridium {

	// Forward declarations
	class VertexArray;
	class Texture;

	struct DrawCall
	{
		SharedPtr<VertexArray> VAO;
		AssetHandle Material;
		Matrix4 Transform;
	};

	struct LightEnvironment
	{
		PointLight PointLights[MAX_POINT_LIGHTS];
		SpotLight SpotLights[MAX_SPOT_LIGHTS];
		DirectionalLight DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
	};

	class SceneRenderer
	{
	public:
		SceneRenderer( const SharedPtr<Scene>& a_Scene );

		void Render( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );

	protected:
		void BeginScene( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );
		void EndScene();

		void Flush();

		void SubmitDrawCall( const DrawCall& a_DrawCall );
		void PerformDrawCall( const DrawCall& a_DrawCall );

	private:
		SharedPtr<Scene> m_Scene;
		SharedPtr<Shader> m_DefaultShader;
		SharedPtr<Material> m_DefaultMaterial;
		SharedPtr<Shader> m_SkyboxShader;
		SharedPtr<VertexArray> m_SkyboxVAO;
		SceneEnvironment& m_SceneEnvironment;

		SharedPtr<Texture> m_BrdfLUT;

		// Per frame data
		Vector3 m_CameraPosition;
		Matrix4 m_ViewMatrix;
		Matrix4 m_ProjectionMatrix;
		Matrix4 m_ViewProjectionMatrix;
		std::vector<DrawCall> m_DrawCalls;
		LightEnvironment m_LightEnvironment;
	};

}