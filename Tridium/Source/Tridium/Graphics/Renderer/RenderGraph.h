#pragma once
#include <Tridium/Common/Function.h>
#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/UnorderedMap.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIBuffer.h>

namespace Tridium {

	//=================================================================================================
	class RenderPass;
	class RenderPassBuilder;
	class RenderGraph;

	//=================================================================================================
	enum class RenderPassID : uint32_t { Invalid = Cast<uint32_t>( ~0 ) };
	enum class RenderPassTextureID : uint32_t { Invalid = Cast<uint32_t>( ~0 ) };
	enum class RenderPassBufferID : uint32_t { Invalid = Cast<uint32_t>( ~0 ) };

	//=================================================================================================
	// Render Pass:
	// A render pass is a single step in the rendering pipeline that executes a set of commands
	//=================================================================================================
	class RenderPass
	{
	public:

		const String& Name() const { return m_Name; }
		ERHICommandQueueType Stage() const { return m_Stage; }

		RenderPassTextureID GetTextureID( StringView a_TextureName )
		{
			if ( auto it = m_Textures.find( a_TextureName ); it != m_Textures.end() )
				return it->second;

			return RenderPassTextureID::Invalid;
		}

		RenderPassBufferID GetBufferID( StringView a_BufferName )
		{
			if ( auto it = m_Buffers.find( a_BufferName ); it != m_Buffers.end() )
				return it->second;

			return RenderPassBufferID::Invalid;
		}

	protected:

		//=============================================================================================
		friend RenderPassBuilder;
		friend RenderGraph;

		//=============================================================================================
		template<typename _ResourceID>
		using ResourceMap = UnorderedMap<String, _ResourceID, TransparentStringHash, TransparentStringEqual>;

		//=============================================================================================
		// Name of the render pass.
		String m_Name;

		//=============================================================================================
		// The type of command queue this pass will use.
		ERHICommandQueueType m_Stage = ERHICommandQueueType::Graphics;

		//=============================================================================================
		// User function to execute the render pass.
		Delegate<void( IRHICommandList&, RenderGraph& )> m_Execute;

		//=============================================================================================
		ResourceMap<RenderPassTextureID> m_Textures{};
		ResourceMap<RenderPassBufferID> m_Buffers{};

	};

	//=================================================================================================
	// Render Pass Builder:
	//=================================================================================================
	class RenderPassBuilder
	{
	public:

		//=============================================================================================
		RenderPassBuilder( RenderGraph& a_Graph, RenderPassID a_PassID ) 
			: m_Graph( a_Graph ), m_PassID( a_PassID ) {}

		//=============================================================================================
		RenderPass* GetPass( const HashedString& a_Name ) const;

		//=============================================================================================
		RenderPassTextureID Import( const RHITextureRef& a_Resource );
		RenderPassBufferID Import( const RHIBufferRef& a_Resource );

		//=============================================================================================
		// Create transient resources owned by the graph
		RenderPassTextureID Create( String a_Name, const RHITextureDesc& a_Desc );
		RenderPassBufferID Create( String a_Name, const RHIBufferDesc& a_Desc );

		//=============================================================================================
		void Read( RenderPassTextureID a_Resource, ERHIResourceStates a_Usage );
		void Write( RenderPassTextureID a_Resource, ERHIResourceStates a_Usage );
		void Read( RenderPassBufferID a_Resource, ERHIResourceStates a_Usage );
		void Write( RenderPassBufferID a_Resource, ERHIResourceStates a_Usage );

		//=============================================================================================
		// Set the execution function for the render pass.
		// WARNING: This function should not capture any variables from the setup function by reference.
		// The render graph may be executed at a later time when those references are no longer valid.
		// If you need to capture variables, use value captures.
		template<std::invocable< IRHICommandList&, RenderGraph&> _Func>
		void Execute( _Func&& a_ExecuteFunc );

	private:

		//=============================================================================================
		RenderGraph& m_Graph;
		RenderPassID m_PassID;

	};

	//=================================================================================================
	// Render Graph:
	// A render graph is a directed acyclic graph (DAG) that describes the dependencies between render passes.
	// It allows for efficient scheduling and execution of render passes based on their dependencies.
	//=================================================================================================
	class RenderGraph
	{
	public:

		//=============================================================================================
		RenderGraph() = default;
		~RenderGraph() = default;

		//=============================================================================================
		RenderGraph( const RenderGraph& ) = delete;
		RenderGraph( RenderGraph&& ) = delete;
		RenderGraph& operator=( const RenderGraph& ) = delete;
		RenderGraph& operator=( RenderGraph&& ) = delete;

		template<std::invocable< RenderPassBuilder& > _Func>
		RenderPassID AddPass( String a_Name, ERHICommandQueueType a_Stage, _Func&& a_SetupAndRecord )
		{
			const RenderPassID id = Cast<RenderPassID>( m_Passes.Size() );

			RenderPassNode& passNode = m_Passes.EmplaceBack();
			RenderPass& pass = passNode.Data;
			pass.m_Name = std::move( a_Name );
			pass.m_Stage = a_Stage;

			RenderPassBuilder builder( *this, id );
			a_SetupAndRecord( builder );

			const HashedString passNameHash = Hashing::HashString( pass.m_Name );
			ASSERT( !m_PassLookup.contains( passNameHash ), "Render pass with the same name already exists!" );
			m_PassLookup[ passNameHash ] = id;

			return id;
		}

		//=============================================================================================
		// Compile the render graph to prepare it for execution.
		void Compile();

		//=============================================================================================
		// Execute the render graph on the given command list.
		void Execute( IRHICommandList& a_CommandList );

		//=============================================================================================
		// Clear temporary state.
		void ResetFrame();

		//=============================================================================================
		const RHITextureRef& GetTexture( const RenderPassTextureID a_ID ) const;
		const RHIBufferRef& GetBuffer( const RenderPassBufferID a_ID ) const;

	protected:

		//=============================================================================================
		friend RenderPassBuilder;

		RenderPassTextureID CreateTexture( const RHITextureDesc& a_Desc, bool a_Imported, const RHITextureRef& a_Resource );
		RenderPassBufferID CreateBuffer( const RHIBufferDesc& a_Desc, bool a_Imported, const RHIBufferRef& a_Resource );

		//=============================================================================================
		// Compiler stages
		void CullPasses();			// Remove passes that do not contribute to externals
		void TopoSort();			// Kahn's algorithm using resource edges
		void ComputeLifetimes();	// Fill FirstUse/LastUse per resource
		void BuildBarriers();		// Translate Access/Usage to RHI barriers
		void AliasTransients();		// Trivial linear allocator MVP (per-type)

	protected:

		struct ResourceEdge 
		{ 
			RenderPassID Source;
			uint32_t Result;
			ERHIObjectType ResultType;
			ERHIResourceStates Usage;
			bool Write;
		};

		struct RenderPassNode
		{
			RenderPass Data;
			// Resources that this pass reads from.
			SmallArray<ResourceEdge, 16> Reads;
			// Resources that this pass writes to.
			SmallArray<ResourceEdge, 16> Writes;
			// Dependent render passes that must be executed before this pass.
			SmallArray<RenderPassID, 8> Dependencies;
			// Render passes that depend on this pass.
			SmallArray<RenderPassID, 8> Dependents;
			// Whether the render pass was culled from the render graph.
			bool Culled = false;
			// Computed during compilation, the first render pass that uses this resource.
			uint32_t InDegree = 0;
		};

		struct ResourceNode
		{
			// Whether the resource was imported from an external source. If true, 'Resource' is provided by the user.
			bool Imported = false;
			// The index of the render pass that first used this resource.
			int32_t FirstUse = INT32_MAX;
			// The index of the render pass that last used this resource.
			int32_t LastUse = -1;
		};

		struct TextureNode : ResourceNode
		{
			// Texture resource descriptor used to create the texture.
			RHITextureDesc Desc{};
			// The physical texture resource. Null until allocated or imported.
			RHITextureRef Resource = nullptr;
		};

		struct BufferNode : ResourceNode
		{
			// Buffer resource descriptor used to create the buffer.
			RHIBufferDesc Desc{};
			// The physical buffer resource. Null until allocated or imported.
			RHIBufferRef Resource = nullptr;
		};

		// List of render passes in the graph.
		Array<RenderPassNode> m_Passes;
		Array<TextureNode> m_Textures;
		Array<BufferNode> m_Buffers;

		// Lookup for render passes by name.
		UnorderedMap<HashedString, RenderPassID> m_PassLookup;

		// Indices into 'm_Passes' representing the execution order of the render passes.
		Array<RenderPassID> m_Schedule;

		friend class RenderPassBuilder;

	};

	//=================================================================================================
	// RenderPassBuilder implementation
	//=================================================================================================

	inline RenderPass* RenderPassBuilder::GetPass( const HashedString& a_Name ) const
	{
		auto it = m_Graph.m_PassLookup.find( a_Name );
		if ( it != m_Graph.m_PassLookup.end() )
		{
			RenderPassID id = it->second;
			if ( m_Graph.m_Passes.IsValidIndex( Cast<size_t>( id ) ) )
			{
				return &m_Graph.m_Passes[ Cast<size_t>( id ) ].Data;
			}
		}

		return nullptr;
	}

	inline RenderPassTextureID RenderPassBuilder::Import( const RHITextureRef& a_Resource )
	{
		return m_Graph.CreateTexture( RHITextureDesc{}, true, a_Resource );
	}

	inline RenderPassBufferID RenderPassBuilder::Import( const RHIBufferRef& a_Resource )
	{
		return m_Graph.CreateBuffer( RHIBufferDesc{}, true, a_Resource );
	}

	inline RenderPassTextureID RenderPassBuilder::Create( String a_Name, const RHITextureDesc& a_Desc )
	{
		const RenderPassTextureID id = m_Graph.CreateTexture( a_Desc, false, RHITextureRef() );
		m_Graph.m_Passes[ Cast<size_t>( m_PassID ) ].Data.m_Textures[ a_Name ] = id;
		return id;
	}

	inline RenderPassBufferID RenderPassBuilder::Create( String a_Name, const RHIBufferDesc& a_Desc )
	{
		const RenderPassBufferID id = m_Graph.CreateBuffer( a_Desc, false, RHIBufferRef() );
		m_Graph.m_Passes[ Cast<size_t>( m_PassID ) ].Data.m_Buffers[ a_Name ] = id;
		return id;
	}

	inline void RenderPassBuilder::Read( RenderPassTextureID a_Resource, ERHIResourceStates a_Usage )
	{
		auto& pass = m_Graph.m_Passes[ Cast<size_t>( m_PassID ) ];
		pass.Reads.EmplaceBack( RenderGraph::ResourceEdge{ m_PassID, Cast<uint32_t>( a_Resource ), ERHIObjectType::Texture, a_Usage, false } );
	}

	inline void RenderPassBuilder::Write( RenderPassTextureID a_Resource, ERHIResourceStates a_Usage )
	{
		auto& pass = m_Graph.m_Passes[ Cast<size_t>( m_PassID ) ];
		pass.Writes.EmplaceBack( RenderGraph::ResourceEdge{ m_PassID, Cast<uint32_t>( a_Resource ), ERHIObjectType::Texture, a_Usage, true } );
	}

	inline void RenderPassBuilder::Read( RenderPassBufferID a_Resource, ERHIResourceStates a_Usage )
	{
		auto& pass = m_Graph.m_Passes[ Cast<size_t>( m_PassID ) ];
		pass.Reads.EmplaceBack( RenderGraph::ResourceEdge{ m_PassID, Cast<uint32_t>( a_Resource ), ERHIObjectType::Buffer, a_Usage, false } );
	}

	inline void RenderPassBuilder::Write( RenderPassBufferID a_Resource, ERHIResourceStates a_Usage )
	{
		auto& pass = m_Graph.m_Passes[ Cast<size_t>( m_PassID ) ];
		pass.Writes.EmplaceBack( RenderGraph::ResourceEdge{ m_PassID, Cast<uint32_t>( a_Resource ), ERHIObjectType::Buffer, a_Usage, true } );
	}

	template<std::invocable<IRHICommandList&, RenderGraph&> _Func>
	inline void RenderPassBuilder::Execute( _Func&& a_ExecuteFunc )
	{

		m_Graph.m_Passes[ Cast<size_t>( m_PassID ) ].Data.m_Execute.Bind( std::forward<_Func>( a_ExecuteFunc ) );
	}


	//=================================================================================================
	// RenderGraph implementation
	//=================================================================================================

	inline const RHITextureRef& RenderGraph::GetTexture( const RenderPassTextureID a_ID ) const
	{
		if ( !m_Textures.IsValidIndex( Cast<size_t>( a_ID ) ) )
		{
			static RHITextureRef NullTexture{};
			return NullTexture;
		}

		return m_Textures[ Cast<size_t>( a_ID ) ].Resource;
	}

	inline const RHIBufferRef& RenderGraph::GetBuffer( const RenderPassBufferID a_ID ) const
	{
		if ( !m_Buffers.IsValidIndex( Cast<size_t>( a_ID ) ) )
		{
			static RHIBufferRef NullBuffer{};
			return NullBuffer;
		}

		return m_Buffers[ Cast<size_t>( a_ID ) ].Resource;
	}

	inline RenderPassTextureID RenderGraph::CreateTexture( const RHITextureDesc& a_Desc, bool a_Imported, const RHITextureRef& a_Resource )
	{
		RenderPassTextureID id = RenderPassTextureID( m_Textures.Size() );
		m_Textures.EmplaceBack( TextureNode{ a_Imported, INT32_MAX, -1, a_Desc, a_Resource }  );
		return id;
	}

	inline RenderPassBufferID RenderGraph::CreateBuffer( const RHIBufferDesc& a_Desc, bool a_Imported, const RHIBufferRef& a_Resource )
	{
		RenderPassBufferID id = RenderPassBufferID( m_Buffers.Size() );
		m_Buffers.EmplaceBack( BufferNode{ a_Imported, INT32_MAX, -1, a_Desc, a_Resource } );
		return id;
	}

} // namespace Tridium