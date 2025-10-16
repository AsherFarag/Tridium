#pragma once
#include <Tridium/Math/BoundingVolume.h>
#include <Tridium/Graphics/Renderer/Lighting.h>
#include <Tridium/Graphics/Renderer/RenderPassTag.h>
#include <Tridium/Graphics/RHI/RHIDefinitions.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>
#include <Tridium/Graphics/RHI/RHIShaderBindings.h>
#include <Tridium/Shaders/RenderView_ShaderInterop.h>

#include <algorithm>
#include <ranges>

namespace Tridium {

	//=============================================================================================
	// Light ID: A unique identifier for lights in the scene.
	//=============================================================================================
	using LightID = int32_t;
	
	//=============================================================================================
	// Draw Item: A singular draw call with all required state to issue it.
	// Draw items are owned by a draw packet and must have an associated draw packet to be rendered.
	//=============================================================================================
	struct DrawItem
	{
		IRHIGraphicsPipelineState* PipelineState = nullptr;
		IRHIBindingSet* BindingSet = nullptr;
		IRHIBuffer* VertexBuffer = nullptr;
		IRHIBuffer* IndexBuffer = nullptr;
		RHIDrawArgs DrawArgs{};
		RenderPassTagMask PassTags{};
		BoundingVolume Volume{};
		Matrix4 Transform{ 1.0f };
	};

	//=============================================================================================
	// Draw Packet: A collection of draw items that can be rendered together.
	// Draw packets are used to group draw items for efficient rendering.
	//=============================================================================================
	struct DrawPacket
	{
		//=========================================================================================
		// Optional name for debugging and profiling purposes.
		String Name{};

		//=========================================================================================
		// The list of draw items contained in this packet.
		Array<DrawItem> Items{};

		//=========================================================================================
		// Resources that need to be kept alive while this packet is being rendered.
		Array<RHIObjectRef> TrackedResources{};

		//=========================================================================================
		// The bounding volume that encompasses all draw items in this packet.
		BoundingVolume Volume{};
	};

	//=============================================================================================
	// Draw Packet Builder: Utility class for constructing draw packets.
	// TODO: This should handle the memory allocations for the draw items and inlined constants.
	//=============================================================================================
	class DrawPacketBuilder
	{
	public:

		//=========================================================================================
		class DrawItemBuilder
		{
		public:

			//=====================================================================================
			DrawItemBuilder( DrawPacketBuilder& a_Owner ) 
				: m_Owner( a_Owner )
			{
				m_Owner.m_Packet.Items.PushBack( {} );
			}

			//=====================================================================================
			auto& SetPipelineState( const RHIGraphicsPipelineStateRef& a_PipelineState )
			{
				m_Owner.m_Packet.Items.Back().PipelineState = a_PipelineState.get();
				if ( a_PipelineState )
				{
					m_Owner.m_Packet.TrackedResources.EmplaceBack( SharedPtrCast<IRHIObject>( a_PipelineState ) );
				}
				return *this;
			}

			//=====================================================================================
			auto& SetBindingSet( const RHIBindingSetRef& a_BindingSet )
			{
				m_Owner.m_Packet.Items.Back().BindingSet = a_BindingSet.get();
				if ( a_BindingSet ) 
					m_Owner.m_Packet.TrackedResources.EmplaceBack( SharedPtrCast<IRHIObject>( a_BindingSet ) );
				return *this;
			}

			//=====================================================================================
			auto& SetVertexBuffer( const RHIBufferRef& a_VertexBuffer )
			{
				m_Owner.m_Packet.Items.Back().VertexBuffer = a_VertexBuffer.get();
				if ( a_VertexBuffer )
					m_Owner.m_Packet.TrackedResources.EmplaceBack( SharedPtrCast<IRHIObject>( a_VertexBuffer ) );
				return *this;
			}

			//=====================================================================================
			auto& SetIndexBuffer( const RHIBufferRef& a_IndexBuffer )
			{
				m_Owner.m_Packet.Items.Back().IndexBuffer = a_IndexBuffer.get();
				if ( a_IndexBuffer )
					m_Owner.m_Packet.TrackedResources.EmplaceBack( SharedPtrCast<IRHIObject>( a_IndexBuffer ) );
				return *this;
			}

			//=====================================================================================
			auto& SetDrawArgs( const RHIDrawArgs& a_DrawArgs )
			{
				m_Owner.m_Packet.Items.Back().DrawArgs = a_DrawArgs;
				return *this;
			}

			auto& SetPassTags( RenderPassTagMask a_PassTags )
			{
				m_Owner.m_Packet.Items.Back().PassTags = a_PassTags;
				return *this;
			}

			//=====================================================================================
			auto& SetVolume( const BoundingVolume& a_Volume )
			{
				m_Owner.m_Packet.Items.Back().Volume = a_Volume;
				//m_Owner.m_Packet.Volume.Expand( a_Volume );
				return *this;
			}

			//=====================================================================================
			auto& SetTransform( const Matrix4& a_Transform )
			{
				m_Owner.m_Packet.Items.Back().Transform = a_Transform;
				return *this;
			}

		private:

			//=====================================================================================
			DrawPacketBuilder& m_Owner;

		};

		//=========================================================================================
		void Reserve( size_t a_ItemCount )
		{
			m_Packet.Items.Reserve( a_ItemCount );
			m_Packet.TrackedResources.Reserve( a_ItemCount * 4 ); // Assume 4 resources per item on average.
		}

		//=========================================================================================
		void SetName( String a_Name )
		{
			m_Packet.Name = std::move( a_Name );
		}

		//=========================================================================================
		[[nodiscard]] DrawItemBuilder AddDrawItem()
		{
			return DrawItemBuilder{ *this };
		}

		//=========================================================================================
		DrawPacket Build()
		{
			return std::move( m_Packet );
		}

	private:

		//=========================================================================================
		DrawPacket m_Packet{};

	};

	//=============================================================================================
	// Render View ID: A unique identifier for views in the scene.
	// These IDs are assigned when views are created 
	// and can be used to retrieve the output of specific views.
	//=============================================================================================
	using RenderViewID = Handle<uint32_t>;

	//=============================================================================================
	// Render View Type: The type of render view,
	// which determines how it is processed by the render pipeline.
	// Custom views can be defined for specialized rendering tasks, 
	// but are ignored by default pipeline processes.
	//=============================================================================================
	enum class ERenderViewType : uint8_t
	{
		None = 0,
		Camera,
		Shadow,
		Custom,
	};

	//=============================================================================================
	// Render View: Represents a specific viewpoint or camera through which draw items are rendered.
	//=============================================================================================
	struct RenderView
	{
		//=========================================================================================
		// View constants the shaders will use when rendering this view.
		RenderViewConstants Constants{};

		//=========================================================================================
		// A constant buffer containing the view constants. If null, it will be created at render time.
		RHIBufferRef ConstantsBuffer = nullptr;

		//=========================================================================================
		// Optional name for debugging and profiling purposes.
		String Name{};

		//=========================================================================================
		// The type of view, which determines how it is processed by the render pipeline.
		ERenderViewType Type = ERenderViewType::None;

		//=========================================================================================
		// Whether this view is enabled and should be rendered.
		bool Enabled = true;

		//=========================================================================================
		// Additional data depending on the view type.
		union
		{
			//=====================================================================================
			struct
			{
				ERHIFormat OutputFormat = ERHIFormat::Unknown;
			} Camera;

			//=====================================================================================
			struct
			{
				//=================================================================================
				// The ID of the light that this shadow map is associated with.
				LightID Light = -1;

				//=================================================================================
				// The type of light casting the shadows (e.g., directional, point, spot).
				ELightType LightType = ELightType::Unknown;
			} Shadow;

			//=====================================================================================
			struct
			{
				//=================================================================================
				// A custom identifier that can be used to differentiate between multiple custom views.
				uint32_t ID = 0;

				//=================================================================================
				// User-defined data associated with the custom view.
				void* UserData = nullptr;
			} Custom;
		};
	};

	//=============================================================================================
	// Render View List: A collection of render views, categorized by type.
	// Provides methods to filter and retrieve views based on their type.
	// Automatically filters out disabled views.
	//=============================================================================================
	struct RenderViewList
	{
		//=========================================================================================
		Span<const Pair<RenderViewID, RenderView>> RawViews{};

		//=========================================================================================
		// Computes the number of enabled views in the list.
		[[nodiscard]] size_t Count() const
		{
			return std::ranges::count_if( RawViews, []( const Pair<RenderViewID, RenderView>& a_View )
			{
				return a_View.second.Enabled;
			} );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled views.
		auto Views() const
		{
			return RawViews | std::views::filter( []( const Pair<RenderViewID, RenderView>& a_View )
			{
				return a_View.second.Enabled;
			} );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled views of the specified type.
		auto ViewsOf( ERenderViewType a_Type ) const
		{
			return RawViews | std::views::filter( [a_Type]( const Pair<RenderViewID, RenderView>& a_View )
			{
				return a_View.second.Enabled && a_View.second.Type == a_Type;
			} );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled camera views.
		auto Cameras() const
		{
			return ViewsOf( ERenderViewType::Camera );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled shadow views.
		auto Shadows() const
		{
			return ViewsOf( ERenderViewType::Shadow );
		}

		//=========================================================================================
		// Returns a filtered view of only the enabled custom views.
		auto CustomViews() const
		{
			return ViewsOf( ERenderViewType::Custom );
		}
	};

	//=============================================================================================
	// Render Context: Encapsulates all data required for rendering a frame,
	// including draw items and lighting information.
	//=============================================================================================
	class RenderContext
	{
	public:

		//=========================================================================================
		const auto& DrawPackets() const { return m_DrawPackets; }
		const auto& Lighting() const { return m_Lighting; }
		const auto& ViewOutputs() const { return m_ViewOutputs; }

		//=========================================================================================
		const RHITextureRef& GetViewOutput( RenderViewID a_ViewID ) const
		{
			static const RHITextureRef s_NullTexture = nullptr;

			auto it = m_ViewOutputs.find( a_ViewID );
			if ( it != m_ViewOutputs.end() )
				return it->second;

			return s_NullTexture;
		}

		//=========================================================================================
		const DirectionalLight* GetDirectionalLight( LightID a_ID )
		{
			if ( a_ID < 0 || a_ID >= Cast<LightID>( m_Lighting.DirectionalLights.Size() ) )
				return nullptr;

			return &m_Lighting.DirectionalLights[a_ID];
		}

		//=========================================================================================
		const PointLight* GetPointLight( LightID a_ID )
		{
			if ( a_ID < 0 || a_ID >= Cast<LightID>( m_Lighting.PointLights.Size() ) )
				return nullptr;

			return &m_Lighting.PointLights[a_ID];
		}

		//=========================================================================================
		const SpotLight* GetSpotLight( LightID a_ID )
		{
			if ( a_ID < 0 || a_ID >= Cast<LightID>( m_Lighting.SpotLights.Size() ) )
				return nullptr;

			return &m_Lighting.SpotLights[a_ID];
		}

	protected:

		//=========================================================================================
		friend class RenderPipelineManager;

		//=========================================================================================
		Array<DrawPacket> m_DrawPackets{};
		LightEnvironment m_Lighting{};
		UnorderedMap<RenderViewID, RHITextureRef> m_ViewOutputs{};

	};

	//=============================================================================================
	// Culler:
	//=============================================================================================
	struct Culler
	{
		const RenderContext& Context;
		Frustum ViewFrustum;

		Culler( const RenderContext& a_Context, const RenderViewConstants& a_ViewConstants )
			: Context( a_Context )
		{
		}

		auto VisiblePackets() const
		{
			return Context.DrawPackets() | std::views::filter( [this]( const DrawPacket& a_Packet )
			{
				TODO( "Implement frustum culling" );
				//return a_Item.Volume.Overlaps( ViewFrustum );
				return true;
			} );
		}
	};

}