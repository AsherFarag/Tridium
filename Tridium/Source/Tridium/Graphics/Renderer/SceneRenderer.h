#pragma once
#include <Tridium/Asset/MeshAsset.h>
#include <Tridium/Asset/MaterialAsset.h>
#include <Tridium/Graphics/RHI/RHIForward.h>
#include <Tridium/Graphics/Renderer/RenderGraph.h>
#include <Tridium/Graphics/Renderer/RenderResource.h>
#include <Tridium/Math/Matrix.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Core/Hash.h>

namespace Tridium {

	//=================================================================================================
	// Scene Camera
	//=================================================================================================
	struct SceneCamera
	{
		Camera Camera;
		Matrix4 Projection;
		Matrix4 View;
		Vector3 Position;
	};

	//=================================================================================================
	// Renderer Tiering Settings: 
	// Settings that control the level of detail and performance optimizations for rendering. 
	// These settings can be adjusted based on the target platform and performance requirements.
	//=================================================================================================
	struct RendererTieringSettings
	{
		float RenderScale = 1.0f; // Scale factor for rendering resolution
	};

	//=================================================================================================
	// Scene Renderer Options: Settings that control how the scene is rendered with the SceneRenderer.
	//=================================================================================================
	struct SceneRendererOptions
	{
		// Render a simple grid in the scene for debugging purposes.
		bool ShowGrid = false;
		// Settings for renderer tiering.
		RendererTieringSettings TieringSettings{};
	};

	//=================================================================================================
	// Scene Render Pass Interface:
	// Interface for creating custom render passes that can be integrated into the rendering pipeline.
	//=================================================================================================
	class ISceneRenderPass
	{
	public:

		//=============================================================================================
		virtual ~ISceneRenderPass() = default;

		//=============================================================================================
		// Gets the owning SceneRenderer.
		// NOTE: This is not valid during the constructor of the render pass.
		class SceneRenderer& GetSceneRenderer() const { ASSERT( m_SceneRenderer ); return *m_SceneRenderer; }

		//=============================================================================================
		// Called before the render graph is compiled.
		// Use this to set up render passes, declare resources, and configure the render graph.
		// 'a_SceneRenderer' will always be valid while this render pass is alive, 
		// so it can be referenced in the RenderPassBuilder::Execute lambda.
		virtual void Setup( RenderGraph& a_RenderGraph ) = 0;

		//=============================================================================================
		// Called if the viewport size has changed.
		// Use this to resize any resources that depend on the viewport size.
		virtual void OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height ) {};

	private:

		//=============================================================================================
		friend class SceneRenderer;
		SceneRenderer* m_SceneRenderer = nullptr;

	};

	//=================================================================================================
	// Scene Renderer:
	// This class is responsible for rendering a scene, including all its objects, lights, and cameras.
	// It handles the rendering pipeline, including setting up shaders, buffers, and rendering commands.
	//=================================================================================================
	class SceneRenderer
	{
	public:

		//=============================================================================================
		// Built-in render pass names.
		//=============================================================================================
		struct Passes
		{
			static constexpr StringView Root = "Root";
			static constexpr StringView GBuffer = "GBuffer";
			static constexpr StringView Lighting = "Lighting";
			static constexpr StringView Transparent = "Transparent";
			static constexpr StringView Skybox = "Skybox";
			static constexpr StringView PostProcess = "PostProcess";
		};

		//=============================================================================================
		SceneRenderer( AssetRef<Scene> a_Scene, const SceneRendererOptions& a_Options = SceneRendererOptions() );
		SceneRenderer( const SceneRenderer& a_Other ) = delete;
		SceneRenderer( SceneRenderer&& a_Other ) = default;
		SceneRenderer& operator=( const SceneRenderer& a_Other ) = delete;
		SceneRenderer& operator=( SceneRenderer&& a_Other ) = default;
		~SceneRenderer();

		//=============================================================================================
		// Prepares the renderer for rendering a scene. 
		// Must be called before rendering any objects and have a matching Close() call.
		void Open( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition );

		//=============================================================================================
		// Finalizes the rendering process.
		void Close();

		//=============================================================================================
		// Has Open() been called and not yet Close()?
		bool IsOpen() const { return m_Open; }

		//=============================================================================================
		// Submits a static mesh for rendering. 
		// SceneRenderer must be open.
		void SubmitStaticMesh( AssetRef<StaticMesh> a_StaticMesh, const Matrix4& a_Transform );

		//=============================================================================================
		// Gets the output texture that contains the rendered scene.
		[[nodiscard]] RHITextureRef GetOutputTexture() const;

		//=============================================================================================
		// Sets the scene to be rendered. Cannot be called while the renderer is open.
		void SetScene( AssetRef<Scene> a_Scene );

		//=============================================================================================
		uint32_t GetViewportWidth() const { return m_Viewport.Width; }
		uint32_t GetViewportHeight() const { return m_Viewport.Height; }

		//=============================================================================================
		// Sets the viewport size for rendering. Does not update during rendering.
		void SetViewportSize( uint32_t a_Width, uint32_t a_Height );

		//=============================================================================================
		// Gets the current camera view that the scene is being rendered from.
		const SceneCamera& GetSceneCamera() const { return m_SceneCamera; }

		//=============================================================================================
		// Adds a custom render pass to the renderer.
		// NOTE: This can be called while IsOpen() is true, 
		// but the render pass will not be used until the next frame and the render graph is rebuilt.
		template<Concepts::Derived<ISceneRenderPass> T, typename... _Args>
		T* AddRenderPass( String a_Name, _Args&&... a_Args );

		//=============================================================================================
		// Retrieves a render pass by name. Returns nullptr if not found.
		[[nodiscard]] ISceneRenderPass* GetRenderPass( StringView a_Name ) const;

		//=============================================================================================
		// Retrieves a render pass by name and casts it to the specified type.
		template<Concepts::Derived<ISceneRenderPass> T>
		[[nodiscard]] T* GetRenderPass( StringView a_Name ) const { return DynamicCast<T*>( GetRenderPass( a_Name ) ); }

		//=============================================================================================
		const auto& GetStaticDrawList() const { return m_StaticDrawLists; }

	protected:

		//=============================================================================================
		void BuildRenderGraph();
		void FlushDrawLists();
		void ClearFrameData();

	protected:

		//=============================================================================================
		struct MeshKey
		{
			AssetID MeshAsset = InvalidAssetID;
			AssetID MaterialAsset = InvalidAssetID;
			uint32_t SubMeshIndex = 0; // Index of the submesh in the source mesh

			auto operator<=>( const MeshKey& a_Other ) const = default;
		};

		//=============================================================================================
		struct StaticMeshDrawCall
		{
			RHIBufferRef VertexBuffer;
			RHIBufferRef IndexBuffer;
			RHIBufferRange VertexBufferRange;
			RHIBufferRange IndexBufferRange;
			RHIGraphicsPipelineStateRef PipelineState;
			RHIBindingSetRef BindingSet;
			SmallArray<Matrix4x3<float>, 8> InstanceTransforms;

			bool Empty() const { return InstanceTransforms.Empty(); }
		};

		using StaticDrawList = Map<MeshKey, StaticMeshDrawCall>;

		//=============================================================================================
		// Render settings for the renderer.
		SceneRendererOptions m_Options;

		//=============================================================================================
		// Is the renderer currently open and ready to render?
		bool m_Open = false;

		//=============================================================================================
		// Flag to indicate if the render graph needs to be rebuilt.
		bool m_RequiresRenderGraphRebuild = true;

		//=============================================================================================
		// The scene being rendered.
		AssetRef<Scene> m_Scene;

		//=============================================================================================
		// Custom and built-in scene render passes added to the renderer.
		UnorderedMap<String, UniquePtr<ISceneRenderPass>, TransparentStringHash, TransparentStringEqual> m_RenderPasses;

		//=============================================================================================
		// Camera Data for rendering.
		SceneCamera m_SceneCamera;

		//=============================================================================================
		// Viewport settings for the renderer.
		struct
		{
			uint32_t Width = 0;          // Width of the viewport
			uint32_t Height = 0;         // Height of the viewport
			float InvWidth = 0.0f;       // Inverse of the width for normalization. 1.0 / Width
			float InvHeight = 0.0f;      // Inverse of the height for normalization. 1.0 / Height
			bool NeedsResize = false;    // Flag to indicate if the viewport needs to be resized

			float AspectRatio() const { return ( Height > 0 ) ? Cast<float>( Width ) / Cast<float>( Height ) : 1.0f; }

		} m_Viewport;

		//=============================================================================================
		// Draw lists for static meshes for different rendering passes.
		struct StaticDrawLists
		{
			StaticDrawList Opaque;
			StaticDrawList Transparent;
			StaticDrawList Shadow;
		} m_StaticDrawLists;

		//=============================================================================================
		// Command list for rendering operations
		RHICommandListRef m_CommandList;

		//=============================================================================================
		// The render graph used for rendering the scene.
		RenderGraph m_RenderGraph;

	};

	//= Built-in Scene Render Passes ========================================================================

	//=================================================================================================
	// Root Pass: The root pass that imports the output texture and prepares for rendering.
	//=================================================================================================
	class RootPass : public ISceneRenderPass
	{
	public:

		//=============================================================================================
		void Setup( RenderGraph& a_RenderGraph ) override;

		//=============================================================================================
		void OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height ) override;

		//=============================================================================================
		// The ID of the output texture which contains the final rendered scene.
		RenderPassTextureID GetOutputID() const { return m_Output.first; }

		//=============================================================================================
		// The output texture which contains the final rendered scene.
		const RHITextureRef& GetOutputTexture() const { return m_Output.second; }

	protected:

		//=============================================================================================
		Pair<RenderPassTextureID, RHITextureRef> m_Output;

	};

	//=================================================================================================
	// GBuffer Pass: Generates the G-Buffer by rendering scene geometry.
	//=================================================================================================
	class GBufferPass : public ISceneRenderPass
	{
	public:

		//=============================================================================================
		void Setup( RenderGraph& a_RenderGraph ) override;

		//=============================================================================================
		void OnViewportResize( RenderGraph& a_RenderGraph, uint32_t a_Width, uint32_t a_Height ) override;

		//=============================================================================================
		// The ID of the position texture which contains world space positions of pixels.
		RenderPassTextureID GetPositionID() const { return m_Position.first; }
		RHITextureRef GetPositionTexture() const { return m_Position.second; }

		//=============================================================================================
		// The ID of the albedo texture which contains base color information of pixels.
		RenderPassTextureID GetAlbedoID() const { return m_Albedo.first; }
		RHITextureRef GetAlbedoTexture() const { return m_Albedo.second; }

		//=============================================================================================
		// The ID of the normal texture which contains world space normals of pixels.
		RenderPassTextureID GetNormalID() const { return m_Normal.first; }
		RHITextureRef GetNormalTexture() const { return m_Normal.second; }

		//=============================================================================================
		// The ID of the metallic-roughness-ambient occlusion texture. The channels are packed as follows:
		// R = Metallic, G = Roughness, B = Ambient Occlusion
		RenderPassTextureID GetMRAOID() const { return m_MetallicRoughnessAO.first; }
		RHITextureRef GetMRAOTexture() const { return m_MetallicRoughnessAO.second; }

		//=============================================================================================
		// The ID of the emission texture which contains emissive color information of pixels.
		RenderPassTextureID GetEmissionID() const { return m_Emission.first; }
		RHITextureRef GetEmissionTexture() const { return m_Emission.second; }

		//=============================================================================================
		// The ID of the depth texture which contains depth information of pixels.
		RenderPassTextureID GetDepthID() const { return m_Depth.first; }
		RHITextureRef GetDepthTexture() const { return m_Depth.second; }

	protected:

		//=============================================================================================
		Pair<RenderPassTextureID, RHITextureRef> m_Position;
		Pair<RenderPassTextureID, RHITextureRef> m_Albedo;
		Pair<RenderPassTextureID, RHITextureRef> m_Normal;
		Pair<RenderPassTextureID, RHITextureRef> m_MetallicRoughnessAO;
		Pair<RenderPassTextureID, RHITextureRef> m_Emission;
		Pair<RenderPassTextureID, RHITextureRef> m_Depth;

	};

	//=================================================================================================
	// Lighting Pass: Performs lighting calculations using the G-Buffer data.
	//=================================================================================================
	class LightingPass : public ISceneRenderPass
	{
	public:

		//=============================================================================================
		void Setup( RenderGraph& a_RenderGraph ) override;

		//=============================================================================================
		// The ID of the output texture which contains the final lit scene.
		RenderPassTextureID GetOutputID() const { return m_Output; }

	protected:

		//=============================================================================================
		RHIGraphicsPipelineStateRef m_PipelineState;
		RHIBufferRef m_QuadVertexBuffer;
		RHIBufferRef m_QuadIndexBuffer;
		RenderPassTextureID m_Output;

	};

	//=================================================================================================
	// Skybox Pass: Renders the skybox in the scene.
	//=================================================================================================
	class SkyboxPass : public ISceneRenderPass
	{
	public:

		//=============================================================================================
		void Setup( RenderGraph& a_RenderGraph ) override;

		//=============================================================================================
		// The ID of the output texture which contains the scene with the skybox rendered.
		RenderPassTextureID GetOutputID() const { return m_Output; }

	protected:

		//=============================================================================================
		RHIGraphicsPipelineStateRef m_PipelineState;
		RHIBufferRef m_CubeVertexBuffer;
		RHIBufferRef m_CubeIndexBuffer;
		RenderPassTextureID m_Output;
		RenderPassTextureID m_Depth;

	};

	inline ISceneRenderPass* SceneRenderer::GetRenderPass( StringView a_Name ) const
	{
		const auto it = m_RenderPasses.find( a_Name );
		return ( it != m_RenderPasses.end() ) ? it->second.get() : nullptr;
	}

	template<Concepts::Derived<ISceneRenderPass> T, typename ..._Args>
	inline T* SceneRenderer::AddRenderPass( String a_Name, _Args && ...a_Args )
	{
		m_RequiresRenderGraphRebuild = true;

		UniquePtr<T> renderPass = MakeUnique<T>( std::forward<_Args>( a_Args )... );
		T* renderPassPtr = renderPass.get();
		m_RenderPasses.emplace( std::move( a_Name ), std::move( renderPass ) );

		renderPassPtr->m_SceneRenderer = this;

		return renderPassPtr;
	}

} // namespace Tridium