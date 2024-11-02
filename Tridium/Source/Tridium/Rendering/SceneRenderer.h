#pragma once
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include <Tridium/Rendering/Lights.h>
#include <Tridium/Rendering/FrameBuffer.h>

namespace Tridium {

	// Forward declarations
	class Scene;
	class SceneEnvironment;
	class VertexArray;
	class Texture;
	class MeshSource;
	class SubMesh;

	namespace Editor { class SceneRendererPanel; }
	// -------------------

	struct DrawCall
	{
		SharedPtr<VertexArray> VAO;
		MaterialHandle Material;
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
		SceneRenderer( Scene& a_Scene );

		void Render( const SharedPtr<Framebuffer>& a_FBO, const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );

	protected:
		void BeginScene( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );
		void EndScene();

		void Flush();

		void SubmitDrawCall( DrawCall&& a_DrawCall );
		void PerformDrawCall( const DrawCall& a_DrawCall );

		void GeometryPass();

	private:
		Scene& m_Scene;
		SceneEnvironment& m_SceneEnvironment;

		// Default assets
		SharedPtr<Shader> m_DefaultShader;
		SharedPtr<Material> m_DefaultMaterial;
		SharedPtr<Shader> m_SkyboxShader;
		SharedPtr<MeshSource> m_CubeMesh;
		SharedPtr<Texture> m_BrdfLUT;
		SharedPtr<Texture> m_WhiteTexture;
		SharedPtr<Texture> m_BlackTexture;
		SharedPtr<Texture> m_NormalTexture;

		// Per frame data
		Vector3 m_CameraPosition;
		Matrix4 m_ViewMatrix;
		Matrix4 m_ProjectionMatrix;
		Matrix4 m_ViewProjectionMatrix;
		std::vector<DrawCall> m_DrawCalls;
		LightEnvironment m_LightEnvironment;

		// Shadows
		SharedPtr<Framebuffer> m_ShadowFBO;
		Vector2 m_ShadowMapSize{1024, 1024};
		SharedPtr<Shader> m_ShadowMapShader;
		Matrix4 m_LightViewProjectionMatrix;

		friend class Editor::SceneRendererPanel;
	};

}