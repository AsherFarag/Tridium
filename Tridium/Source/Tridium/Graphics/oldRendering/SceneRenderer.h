#pragma once
#include "DrawList.h"
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include <Tridium/Graphics/oldRendering/Lights.h>
#include <Tridium/Graphics/oldRendering/FrameBuffer.h>
#include <Tridium/Graphics/oldRendering/Pipeline.h>
#include <Tridium/Graphics/oldRendering/GBuffer.h>

namespace Tridium {

	// Forward declarations
	class Scene;
	struct SceneEnvironment;
	class VertexArray;
	class TextureOld;
	class MeshSource;
	class OldSubMesh;
	// -------------------

	struct DrawCall
	{
		SharedPtr<VertexArray> VAO;
		MaterialHandle Material;
		Matrix4 Transform;
	};

	struct LightEnvironment
	{
		OldPointLight PointLights[MAX_POINT_LIGHTS];
		OldSpotLight SpotLights[MAX_SPOT_LIGHTS];
		OldDirectionalLight DirectionalLights[MAX_DIRECTIONAL_LIGHTS];

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
		double RenderTime = 0.0;
		double DrawListGenerationTime = 0.0;
		double GeometryTime = 0.0;
		double ShadowMapTime = 0.0;
		double PostProcessTime = 0.0;

		uint32_t NumDrawCalls = 0u;
		uint32_t CulledDrawCalls = 0u;
	};

	class OldSceneRenderer
	{
	public:
		OldSceneRenderer( Scene& a_Scene );

		void Render( const SharedPtr<Framebuffer>& a_RenderTarget, const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );

		const RenderSettings& GetRenderSettings() const { return m_RenderSettings; }
		void SetRenderSettings( const RenderSettings& a_RenderSettings ) { m_RenderSettings = a_RenderSettings; }

		const RenderStats& GetRenderStats() const { return m_RenderStats; }

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

		void DrawCall( const SharedPtr<VertexArray>& a_VAO );

	private:
		RenderSettings m_RenderSettings;

		Scene& m_Scene;
		SceneEnvironment& m_SceneEnvironment;

		// Default assets
		SharedPtr<Shader> m_DefaultShader;
		SharedPtr<OldMaterial> m_DefaultMaterial;
		SharedPtr<Shader> m_SkyboxShader;
		SharedPtr<MeshSource> m_CubeMesh;
		SharedPtr<TextureOld> m_BrdfLUT;
		SharedPtr<TextureOld> m_WhiteTexture;
		SharedPtr<TextureOld> m_BlackTexture;
		SharedPtr<TextureOld> m_NormalTexture;
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
			Frustum CameraFrustum;
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
		friend class SceneRendererPanel;
	};

}