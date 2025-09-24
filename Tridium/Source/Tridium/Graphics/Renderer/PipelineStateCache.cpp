#include "tripch.h"
#include "PipelineStateCache.h"
#include <Tridium/Common/TimeStamp.h>
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium {

	template<typename _PipelineStateRef>
    struct CachedPipelineState
    {
		TimeStamp LastUsed; TODO( "Implement LRU cache eviction based on LastUsed timestamp" );
        _PipelineStateRef State;
	};

	static UnorderedMap<hash64_t, CachedPipelineState<RHIGraphicsPipelineStateRef>> s_GraphicsPipelineStates;

    hash64_t PipelineStateCache::CreatePipelineID( const RHIGraphicsPipelineStateDesc& a_Desc )
    {
		return std::hash<RHIGraphicsPipelineStateDesc>{}( a_Desc );
    }

    const RHIGraphicsPipelineStateRef& PipelineStateCache::GetGraphicsPSO( hash64_t a_PipelineID )
    {
        auto it = s_GraphicsPipelineStates.find( a_PipelineID );

        if ( it == s_GraphicsPipelineStates.end() )
        {
            static RHIGraphicsPipelineStateRef s_InvalidPipelineState = nullptr;
            return s_InvalidPipelineState;
        }

        return it->second.State;
    }

    const RHIGraphicsPipelineStateRef& PipelineStateCache::GetOrCreatePSO( const RHIGraphicsPipelineStateDesc& a_Desc )
    {
		// Compute the hash ID for the pipeline state
        const hash64_t pipelineID = CreatePipelineID( a_Desc );

        if ( const RHIGraphicsPipelineStateRef& existingState = GetGraphicsPSO( pipelineID ) )
        {
            return existingState;
        }

		// Create a new pipeline state if it doesn't exist and cache it
		return s_GraphicsPipelineStates[pipelineID].State = RHI::CreateGraphicsPipelineState( a_Desc );
    }

    bool PipelineStateCache::Init()
    {
        return true;
    }

    void PipelineStateCache::Shutdown()
    {
		s_GraphicsPipelineStates.clear();
	}

} // namespace Tridium