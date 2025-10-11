#pragma once
#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/String.h>
#include <Tridium/Graphics/Renderer/RenderPassTag.h>
#include <Tridium/Graphics/RHI/RHIDefinitions.h>
#include <Tridium/Shaders/RenderView_ShaderInterop.h>

#include <algorithm>
#include <ranges>

namespace Tridium {

	//=============================================================================================
	// Draw Item: A singular draw call with all required state to issue it.
	//=============================================================================================
	struct DrawItem
	{
		IRHIGraphicsPipelineState* PipelineState = nullptr;
		IRHIBindingSet* BindingSet = nullptr;
		IRHIBuffer* VertexBuffer = nullptr;
		IRHIBuffer* IndexBuffer = nullptr;
		RHIDrawArgs DrawArgs{};
		RenderPassTagMask PassTags{};
		AABB Bounds{};
	};

	//=============================================================================================
	// Render View: Represents a specific viewpoint or camera through which draw items are rendered.
	//=============================================================================================
	struct RenderView
	{
		enum EType : uint8_t
		{
			None = 0,
			Camera = 1 << 0,
			Shadow = 1 << 1,
			Custom = 1 << 2,
		};

		RenderViewConstants Constants{};
		String Name{};
		EType Type = EType::None;
		bool Enabled = true;
	};

	//=============================================================================================
	// Render View List: A collection of render views, categorized by type.
	// Provides methods to filter and retrieve views based on their type.
	// Automatically filters out disabled views.
	//=============================================================================================
	struct RenderViewList
	{
		//=========================================================================================
		Span<const Pair<RenderView, RHIBufferRef>> RawViews{};

		//=========================================================================================
		// Computes the number of enabled views in the list.
		[[nodiscard]] size_t Count() const 
		{ 
			return std::ranges::count_if( RawViews, []( const auto& a_View ) 
			{ 
				return a_View.first.Enabled; 
			} );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled views.
		auto Views() const
		{
			return RawViews | std::views::filter( []( const auto& a_View ) 
			{ 
				return a_View.first.Enabled; 
			} );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled views of the specified type.
		auto ViewsOf( RenderView::EType a_Type ) const
		{
			return RawViews | std::views::filter( [a_Type]( const auto& a_View ) 
			{ 
				return ( a_View.first.Type & a_Type ) != 0 && a_View.first.Enabled;
			} );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled camera views.
		auto Cameras() const
		{
			return ViewsOf( RenderView::Camera );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled shadow views.
		auto Shadows() const
		{
			return ViewsOf( RenderView::Shadow );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled custom views.
		auto CustomViews() const
		{
			return ViewsOf( RenderView::Custom );
		}
	};

	//=============================================================================================
	// IRenderPipeline Interface: Manages the sequence of rendering operations,
	// including multiple render passes, and handles the rendering of views and draw items.
	// There's a render pipeline instance for each frame in flight, up to RHIConstants::MaxFramesInFlight.
	//=============================================================================================
	class IRenderPipeline
	{
	public:

		//=========================================================================================
		NON_COPYABLE_OR_MOVABLE( IRenderPipeline );
		IRenderPipeline() = default;
		virtual ~IRenderPipeline() = default;

	protected:

		//=========================================================================================
		friend class RenderPipelineManager;

		//=========================================================================================
		// Used by the RendererModule to create a new instance of the render pipeline per frame in flight.
		virtual UniquePtr<IRenderPipeline> Create() const = 0;

		//=========================================================================================
		// Renders all views and returns a fence value that signals when rendering is complete.
		virtual RHIFenceValue Render( RenderViewList a_Views ) = 0;

	protected:

		//=========================================================================================
		// Referenced resources used by the draw items.
		UnorderedMap<IRHIObject*, RHIObjectRef> m_TrackedObjects{};

		//=========================================================================================
		// The draw items to be rendered by the pipeline.
		Array<DrawItem> m_DrawItems{};

	};

	//=============================================================================================
	// Render Pipeline Manager:
	//=============================================================================================
	class RenderPipelineManager final
	{
	public:

		//=========================================================================================
		// Returns the render pipeline for the specified frame index (or the current frame if ~0u).
		// WARNING: Do not hold onto this pointer beyond the current frame,
		// as it may become invalid in the next frame.
		IRenderPipeline* GetRenderPipeline( uint32_t a_FrameIndex = ~0u );

		//=========================================================================================
		// Adds a new view to be rendered by the pipeline.
		// Only effective before the first call to Render().
		void AddView( const RenderView& a_View );

	private:

		//=========================================================================================
		friend class RendererModule;

		//=========================================================================================
		void BeginFrame();
		void EndFrame();

	private:

		//=========================================================================================
		using RenderViewStorage = Array<Pair<RenderView, RHIBufferRef>>;

		//=========================================================================================
		struct RenderPipelineInFlight
		{
			RHIFenceValue FenceValue = 0;
			UniquePtr<IRenderPipeline> Pipeline;
		};

		//=========================================================================================
		// A list of render pipelines, one per frame in flight.
		InlineArray<RenderPipelineInFlight, RHIConstants::MaxFramesInFlight> m_RenderPipelines;

		//=========================================================================================
		// The views to render from, along with their associated constant buffers.
		RenderViewStorage m_Views{};

		//=========================================================================================
		// Any views added while rendering is in progress are stored here to be applied next frame.
		RenderViewStorage m_ViewsNextFrame{};

	};

}