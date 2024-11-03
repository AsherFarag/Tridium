#pragma once
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include <Tridium/Rendering/Lights.h>
#include <Tridium/Rendering/FrameBuffer.h>
#include <Tridium/Rendering/Pipeline.h>
#include <Tridium/Rendering/GBuffer.h>

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

		void Render( const SharedPtr<Framebuffer>& a_RenderTarget, const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );

		ERenderMode GetRenderMode() const { return m_RenderMode; }
		void SetRenderMode( ERenderMode a_RenderMode ) { m_RenderMode = a_RenderMode; }

	protected:
		void BeginScene( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );
		void EndScene();

		void Flush();
		void GenerateShadowMaps();
		void SubmitDrawCall( DrawCall&& a_DrawCall );

		// - Deferred Rendering -
		void DeferredRenderPass();
		void DeferredGeometryPass();
		// ----------------------

		// - Forward Rendering -
		void ForwardRenderPass();
		void ForwardGeometryPass();
		// ----------------------

		void RenderSkybox();

		void PostProcessPass();

	private:
		ERenderMode m_RenderMode = ERenderMode::Forward;

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
		// ---------------

		// Per frame data
		struct SceneInfo
		{
			Matrix4 ProjectionMatrix;
			Matrix4 ViewMatrix;
			Matrix4 ViewProjectionMatrix;
			Vector3 CameraPosition;
			Camera Camera;
		} m_SceneInfo;
		std::vector<DrawCall> m_DrawCalls;
		LightEnvironment m_LightEnvironment;
		SharedPtr<Framebuffer> m_RenderTarget;
		// ---------------

		struct DeferredData
		{
			GBuffer GBuffer{ 1280u, 720u };
		} m_DeferredData;

		struct ForwardData
		{
		} m_ForwardData;

		// Shadows
		SharedPtr<Framebuffer> m_ShadowFBO;
		Vector2 m_ShadowMapSize{1024, 1024};
		SharedPtr<Shader> m_ShadowMapShader;
		Matrix4 m_LightViewProjectionMatrix{ 1.0f };
		SharedPtr<Shader> m_ShadowCubeMapShader;
		// -------

		friend class Editor::SceneRendererPanel;
	};

}