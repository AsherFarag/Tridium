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

	constexpr uint32_t MAX_POINT_LIGHTS = 4;

	struct PointLight
	{
		Vector3 Position;
		Vector3 Color;
		float Intensity;
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
		RenderEnvironment& m_RenderEnvironment;

		// Per frame data
		Vector3 m_CameraPosition;
		Matrix4 m_ViewMatrix;
		Matrix4 m_ProjectionMatrix;
		Matrix4 m_ViewProjectionMatrix;
		std::vector<DrawCall> m_DrawCalls;
		PointLight m_PointLights[MAX_POINT_LIGHTS];
	};

}