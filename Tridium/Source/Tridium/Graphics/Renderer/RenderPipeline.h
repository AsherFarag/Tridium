#pragma once
#include <Tridium/Asset/MeshAsset.h>
#include <Tridium/Asset/MaterialAsset.h>
#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/String.h>
#include <Tridium/Graphics/Renderer/RenderGraph.h>
#include <Tridium/Graphics/Renderer/RenderPipelinePass.h>

namespace Tridium {

	//=================================================================================================
	// IRenderPipeline Interface: Manages the sequence of rendering operations,
	// including multiple render passes, and handles the rendering of views and draw items.
	// There's a render pipeline instance for each frame in flight, up to RHIConstants::MaxFramesInFlight.
	//=================================================================================================
	class IRenderPipeline
	{
	public:

		struct Passes
		{
			static constexpr HashedString Root = "Root"_H;
		};

		//=============================================================================================
		NON_COPYABLE_OR_MOVABLE( IRenderPipeline );
		IRenderPipeline() = default;
		virtual ~IRenderPipeline() = default;

		//=============================================================================================
		// Adds a custom render pass to the renderer.
		// Returns true if the pass was added, false if a pass with the same name already exists
		// and 'a_Overwrite' is false.
		bool AddRenderPass( String a_Name, UniquePtr<IRenderPipelinePass> a_Pass, bool a_Overwrite = false )
		{
			if ( a_Pass == nullptr )
			{
				return false;
			}

			if ( !a_Overwrite && m_RenderPasses.contains( a_Name ) )
			{
				return false;
			}

			a_Pass->m_RenderPipeline = this;
			m_RenderPasses[a_Name] = std::move( a_Pass );
			m_RenderGraphRequiresRebuild = true;

			return true;
		}

		//=============================================================================================
		// Adds a custom render pass to the renderer.
		template<Concepts::Derived<IRenderPipelinePass> T, typename... _Args>
		T* AddRenderPass( String a_Name, const _Args&... a_Args )
		{
			if ( m_RenderPasses.contains( a_Name ) )
			{
				return nullptr;
			}

			T* pass = new T( a_Args... );
			AddRenderPass( std::move( a_Name ), UniquePtr<T>( pass ), true );
			return pass;
		}

		//=============================================================================================
		// Retrieves a render pass by name. Returns nullptr if not found.
		[[nodiscard]] IRenderPipelinePass* GetRenderPass( HashedString a_Name ) const
		{
			const auto it = m_RenderPasses.find( a_Name.String() );

			if ( it != m_RenderPasses.end() )
			{
				return it->second.get();
			}

			return nullptr;
		}

		//=============================================================================================
		// Retrieves a render pass by name and casts it to the specified type.
		template<Concepts::Derived<IRenderPipelinePass> T>
		[[nodiscard]] T* GetRenderPass( HashedString a_Name ) const { return DynamicCast<T*>( GetRenderPass( a_Name ) ); }

	protected:

		//=============================================================================================
		// Used by the RendererModule to create a new instance of the render pipeline per frame in flight.
		virtual UniquePtr<IRenderPipeline> Create() const = 0;

		//=============================================================================================
		// Sets up the render pipeline, including initializing render passes and resources.
		// Called once when the pipeline is created.
		virtual bool Setup() = 0;

		//=============================================================================================
		// This is called after 'Setup' which builds and compiles the render graph and all passes.
		void BuildRenderGraph();

		//=============================================================================================
		RHIFenceValue Flush( const RenderContext& a_Context, RenderViewList a_Views );

		//=============================================================================================
		// Renders all views and returns a fence value that signals when rendering is complete.
		virtual RHIFenceValue Render( const RenderContext& a_Context, RenderViewList a_Views ) = 0;

		//=============================================================================================
		// Helper method to render a shadow map for a given view.
		void RenderShadowMap( const RenderContext& a_Context, const RenderView& a_View );

		//=============================================================================================
		// Tracks an RHI object to ensure it remains alive for the duration of the frame.
		void TrackObject( RHIObjectRef a_Object )
		{
			if ( a_Object != nullptr )
			{
				m_TrackedObjects[a_Object.get()] = std::move( a_Object );
			}

		}

	protected:

		//=============================================================================================
		friend class RenderPipelineManager;

		//=============================================================================================
		// Custom and built-in render passes added to the renderer.
		UnorderedMap<String, UniquePtr<IRenderPipelinePass>> m_RenderPasses;

		//=============================================================================================
		// Referenced resources used by the draw items.
		UnorderedMap<IRHIObject*, RHIObjectRef> m_TrackedObjects{};

		//=============================================================================================
		// The render graph used to manage render passes and resource dependencies.
		RenderGraph m_RenderGraph{};
		bool m_RenderGraphRequiresRebuild = true;

	};

	//=================================================================================================
	// Render Pipeline Manager:
	//=================================================================================================
	class RenderPipelineManager final
	{
	public:

		//=============================================================================================
		// Returns the render pipeline for the specified frame index (or the current frame if ~0u).
		// WARNING: Do not hold onto this pointer beyond the current frame,
		// as it may become invalid in the next frame.
		[[nodiscard]] IRenderPipeline* GetRenderPipeline( uint32_t a_FrameIndex = ~0u );

		//=============================================================================================
		bool SetRenderPipeline( UniquePtr<IRenderPipeline> a_Pipeline );

		//=============================================================================================
		// Adds a new view to be rendered by the pipeline.
		// Only effective before the first call to Render().
		void AddView( RenderView a_View )
		{
			m_ViewsNextFrame.EmplaceBack( std::move( a_View ) );
		}

		//=============================================================================================
		// Adds a custom render pass to the pipelines in flight.
		template<Concepts::Derived<IRenderPipelinePass> T, typename... _Args>
		void AddRenderPass( String a_Name, const _Args&... a_Args )
		{
			for ( RenderPipelineInFlight& pipelineInFlight : m_RenderPipelines )
			{
				pipelineInFlight.Pipeline->AddRenderPass( a_Name, MakeUnique<T>( a_Args... ) );
			}
		}

		//=============================================================================================
		// Renders the current frame.
		RHIFenceValue Render();

		//=============================================================================================
		void SubmitDrawPacket( DrawPacket&& a_Packet )
		{ 
			m_RenderContext.m_DrawPackets.EmplaceBack( std::move( a_Packet ) ); 
		}

		//=============================================================================================
		// Creates and submits a draw packet for the given static mesh with the specified transform.
		void SubmitStaticMesh( AssetRef<StaticMesh> a_Mesh, const Matrix4& a_Transform );

		//=============================================================================================
		//
		void SetLightEnvironment( LightEnvironment a_LightEnv )
		{
			m_LightEnvironmentDirty = true;
			m_RenderContext.m_Lighting = std::move( a_LightEnv );
		}

	private:

		//=============================================================================================
		friend class RendererModule;

		//=============================================================================================
		void BeginFrame();
		void EndFrame();

		//=============================================================================================
		void Reset();

	private:

		//=============================================================================================
		struct RenderPipelineInFlight
		{
			RHIFenceValue FenceValue = 0;
			UniquePtr<IRenderPipeline> Pipeline;
		};

		//=============================================================================================
		// A list of render pipelines, one per frame in flight.
		InlineArray<RenderPipelineInFlight, RHIConstants::MaxFramesInFlight> m_RenderPipelines;

		//=============================================================================================
		// The views to render from for the current frame.
		Array<RenderView> m_Views{};

		//=============================================================================================
		// Any views added while rendering is in progress are stored here to be applied next frame.
		Array<RenderView> m_ViewsNextFrame{};

		//=============================================================================================
		bool m_LightEnvironmentDirty = false;

		//=============================================================================================
		RenderContext m_RenderContext{};

		//=============================================================================================
		DrawPacketBuilder m_DrawPacketBuilder{};

	};

} // namespace Tridium