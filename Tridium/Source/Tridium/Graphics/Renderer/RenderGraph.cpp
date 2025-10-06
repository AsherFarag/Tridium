#include "tripch.h"
#include "RenderGraph.h"
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium {

	void RenderGraph::Compile()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		CullPasses();
		TopoSort();
		ComputeLifetimes();
		BuildBarriers();
		AliasTransients();
	}

	void RenderGraph::Execute( IRHICommandList& a_CommandList )
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		for ( const RenderPassID passID : m_Schedule )
		{
			RenderPass& pass = m_Passes[ Cast<size_t>( passID ) ].Data;

			if ( pass.m_Execute )
			{
				a_CommandList.PushDebugGroup( pass.m_Name.c_str() );
				pass.m_Execute( a_CommandList, *this );
				a_CommandList.PopDebugGroup();
			}
		}
	}

	void RenderGraph::ResetFrame()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );
		m_Passes.Clear();
		m_PassLookup.clear();
		m_Textures.Clear();
		m_Buffers.Clear();
		m_Schedule.Clear();
	}

	//=============================================================================================
	// Compiler stages
	//=============================================================================================

	void RenderGraph::CullPasses()
	{
	}

	void RenderGraph::TopoSort()
	{
		TODO( "Optimize this and clean it up!" );

		// Build dependencies
		{
			// For each resource, connect writers -> readers that come after.
			TODO( "Optimize me!" );
			auto Connect = [ & ]( Span<ResourceEdge> a_Reads, Span<ResourceEdge> a_Writes )
			{
				for ( auto& write : a_Writes )
				{
					for ( auto& read : a_Reads )
					{
						if ( write.Result == read.Result && write.ResultType == read.ResultType )
						{
							if ( read.Source > write.Source ) // later pass depends on earlier pass
							{
								// Register dependency to reader
								auto& readerPassNode = m_Passes[ Cast<size_t>( read.Source ) ];
								readerPassNode.Dependencies.PushBack( write.Source );
								readerPassNode.InDegree++;

								// Register dependent to writer
								auto& writerPassNode = m_Passes[ Cast<size_t>( write.Source ) ];
								writerPassNode.Dependents.PushBack( read.Source );
							}
						}
					}
				}
			};


			// Build using per-pass lists
			for ( size_t i = 0; i < m_Passes.Size(); ++i )
			{
				for ( size_t j = 0; j < m_Passes.Size(); ++j )
				{
					if ( i == j ) 
						continue;

					Connect( m_Passes[ j ].Reads, m_Passes[ i ].Writes );
					Connect( m_Passes[ j ].Writes, m_Passes[ i ].Writes ); // WAR/WAW => dep as well
				}
			}
		}


		// Kahn's algorithm
		Array<uint32_t> q; 
		q.Reserve( m_Passes.Size() );

		for ( uint32_t i = 0; i < m_Passes.Size(); ++i ) 
		{
			if ( m_Passes[ i ].InDegree == 0 )
				q.PushBack( i );
		}


		m_Schedule.Clear();
		m_Schedule.Reserve( m_Passes.Size() );

		// Kahn's algorithm: process nodes with no incoming edges
		while ( !q.Empty() )
		{
			uint32_t i = q.Back(); q.PopBack();
			m_Schedule.PushBack( Cast<RenderPassID>( i ) );

			// Walk dependents (outgoing edges)
			auto& passNode = m_Passes[ i ];
			for ( const RenderPassID dependentID : passNode.Dependents )
			{
				RenderPassNode& dependentNode = m_Passes[ Cast<size_t>( dependentID ) ];

				if ( dependentNode.InDegree > 0 && --dependentNode.InDegree == 0 )
					q.PushBack( Cast<size_t>( dependentID ) );
			}
		}

		// If schedule size < passes, there was a cycle (should not happen in correct RG)
		ENSURE( m_Schedule.Size() == m_Passes.Size(), "RenderGraph: Cycle detected in render passes!" );
	}

	void RenderGraph::ComputeLifetimes()
	{
		// Iterate schedule indices after topoSort; for MVP we compute after scheduling
		for ( int32_t i = 0; i < Cast<int32_t>( m_Passes.Size() ); ++i )
		{
			RenderPassNode& pass = m_Passes[ i ];

			for ( auto& edge : pass.Reads )
			{ 
				ResourceNode& n = ( edge.ResultType == ERHIObjectType::Texture ) ? Cast<ResourceNode&>( m_Textures[ edge.Result ] ) : Cast<ResourceNode&>( m_Buffers[ edge.Result ] );
				n.FirstUse = Math::Min( n.FirstUse, i );
				n.LastUse = Math::Max( n.LastUse, i ); 
			}

			for ( auto& edge : pass.Writes ) 
			{
				ResourceNode& n = ( edge.ResultType == ERHIObjectType::Texture ) ? Cast<ResourceNode&>( m_Textures[ edge.Result ] ) : Cast<ResourceNode&>( m_Buffers[ edge.Result ] );
				n.FirstUse = Math::Min( n.FirstUse, i );
				n.LastUse = Math::Max( n.LastUse, i );
			}
		}
	}

	void RenderGraph::BuildBarriers()
	{
	}

	void RenderGraph::AliasTransients()
	{
		// Lazily allocate on first use, free on last use

		for ( auto& texNode : m_Textures )
		{
			if ( texNode.Imported || texNode.Resource )
				continue;

			texNode.Resource = RHI::CreateTexture( texNode.Desc );
		}

		for ( auto& bufferNode : m_Buffers )
		{
			if ( bufferNode.Imported || bufferNode.Resource )
				continue;

			bufferNode.Resource = RHI::CreateBuffer( bufferNode.Desc );
		}
	}


}
