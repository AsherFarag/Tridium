#pragma once
#include <Tridium/Graphics/RHI/RHIPipelineState.h>

namespace Tridium {

	//=================================================================================================
	// Pipeline State Cache: Caches pipeline state objects (PSOs) for quick retrieval and reuse.
	//=================================================================================================
	class PipelineStateCache
	{
	public:

		//=============================================================================================
		NON_COPYABLE_OR_MOVABLE( PipelineStateCache );

		//=============================================================================================
		static hash64_t CreatePipelineID( const RHIGraphicsPipelineStateDesc& a_Desc );

		//=============================================================================================
		static const RHIGraphicsPipelineStateRef& GetGraphicsPSO( hash64_t a_PipelineID );

		//=============================================================================================
		static const RHIGraphicsPipelineStateRef& GetOrCreatePSO( const RHIGraphicsPipelineStateDesc& a_Desc );

	private:

		//=============================================================================================
		friend class RendererModule;
		static bool Init();
		static void Shutdown();

	};

} // namespace Tridium