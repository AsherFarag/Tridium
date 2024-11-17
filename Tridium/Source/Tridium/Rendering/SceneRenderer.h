#pragma once
#include "DrawList.h"
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

		uint32_t NumPointLights = 0u;
		uint32_t NumSpotLights = 0u;
		uint32_t NumDirectionalLights = 0u;
	};

	struct RenderSettings
	{
		ERenderMode RenderMode = ERenderMode::Deferred;
		float RenderScale = 1.0f;

		// Temp?
		bool DebugDrawColliders = false;
	};

	struct RenderStats
	{
		uint32_t NumDrawCalls = 0u;
	};

	class SceneRenderer
	{
	public:
		SceneRenderer( Scene& a_Scene );

		void Render( const SharedPtr<Framebuffer>& a_RenderTarget, const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );

		const RenderSettings& GetRenderSettings() const { return m_RenderSettings; }
		void SetRenderSettings( const RenderSettings& a_RenderSettings ) { m_RenderSettings = a_RenderSettings; }

	protected:
		void BeginScene( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );
		void EndScene();

		void Clear(); // Clears all the per frame data
		void GenerateShadowMaps();

		// - Deferred Rendering -
		void DeferredRenderPass();
		void DeferredGBufferPass();
		void DeferredLightingPass();
		// ----------------------

		// - Forward Rendering -
		void ForwardRenderPass();
		void ForwardGeometryPass();
		// ----------------------

		void RenderSkybox();
		void PostProcessPass();

		void DebugRenderColliders();

	private:
		RenderSettings m_RenderSettings;

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

		// Debug assets
		SharedPtr<Shader> m_DebugSimpleShader;
		SharedPtr<VertexArray> m_DebugSphereVAO;
		SharedPtr<VertexArray> m_DebugCubeVAO;
		SharedPtr<VertexArray> m_DebugCapsuleVAO;
		SharedPtr<VertexArray> m_DebugCylinderVAO;
		// ------------

		// Per frame data
		struct SceneInfo
		{
			Matrix4 ProjectionMatrix;
			Matrix4 ViewMatrix;
			Matrix4 ViewProjectionMatrix;
			Vector3 CameraPosition;
			Camera Camera;
		} m_SceneInfo;
		DrawList m_DrawList;
		LightEnvironment m_LightEnvironment;
		SharedPtr<Framebuffer> m_RenderTarget;
		RenderStats m_RenderStats;
		// ---------------

		struct DeferredData
		{
			GBuffer GBuffer{ 1280u, 720u };
			SharedPtr<Shader> GBufferShader;
			SharedPtr<Shader> LightingShader;
			SharedPtr<VertexArray> QuadVAO;
		} m_DeferredData;

		struct ForwardData
		{
		} m_ForwardData;

		// Shadows
		SharedPtr<Shader> m_ShadowMapShader;
		SharedPtr<Shader> m_ShadowCubeMapShader;
		// -------
		
		friend class Scene;
		friend class Editor::SceneRendererPanel;
	};

}