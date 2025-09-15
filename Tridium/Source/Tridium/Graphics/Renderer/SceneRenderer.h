#pragma once
#include <Tridium/Asset/MeshAsset.h>
#include <Tridium/Asset/MaterialAsset.h>
#include <Tridium/Graphics/RHI/RHIForward.h>
#include <Tridium/Graphics/Renderer/RenderGraph.h>
#include <Tridium/Math/Matrix.h>
#include <Tridium/Scene/Scene.h>

namespace Tridium {

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
	// Scene Renderer:
	// This class is responsible for rendering a scene, including all its objects, lights, and cameras.
	// It handles the rendering pipeline, including setting up shaders, buffers, and rendering commands.
	//=================================================================================================
	class SceneRenderer
	{
	public:

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
		RHITextureRef GetOutputTexture() const;

		//=============================================================================================
		// Sets the scene to be rendered. Cannot be called while the renderer is open.
		void SetScene( AssetRef<Scene> a_Scene );

		//=============================================================================================
		// Sets the viewport size for rendering. Does not update during rendering.
		void SetViewportSize( uint32_t a_Width, uint32_t a_Height );

	protected:

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
			AssetRef<StaticMesh> StaticMesh;                      // The static mesh to render
			SmallArray<Matrix4x3<float>, 8> InstanceTransforms;   // Transforms for each instance of the static mesh
			uint32_t InstanceCount = 0;                           // Number of instances to render
		};

		using StaticDrawList = Map<MeshKey, StaticMeshDrawCall>;

		//=============================================================================================
		// Render settings for the renderer.
		SceneRendererOptions m_Options;

		//=============================================================================================
		// Is the renderer currently open and ready to render?
		bool m_Open = false;

		//=============================================================================================
		// The scene being rendered.
		AssetRef<Scene> m_Scene;

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
		// Camera Data for rendering.
		struct
		{
			Camera Camera;               // The camera used for rendering
			Matrix4 View;                // View matrix for the camera
			Vector3 Position;            // Position of the camera in world space
		} m_CameraData;

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

		//=============================================================================================
		// The output texture for the renderer used by the render graph.
		RHITextureRef m_OutputTexture;

	};

} // namespace Tridium