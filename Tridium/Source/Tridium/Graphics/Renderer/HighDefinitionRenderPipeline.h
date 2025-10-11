#pragma once
#include <Tridium/Graphics/Renderer/RenderPipeline.h>

namespace Tridium {

	//=============================================================================================
	// High Definition Render Pipeline (HDRP):
	// An advanced rendering pipeline that supports high-fidelity graphics and effects.
	// Uses physically based rendering (PBR) techniques, advanced lighting models,
	// and post-processing effects to achieve realistic visuals.
	//=============================================================================================
	class HighDefinitionRenderPipeline : public IRenderPipeline
	{
	public:

		//=========================================================================================
		HighDefinitionRenderPipeline() = default;
		~HighDefinitionRenderPipeline() override = default;

	protected:

		//=========================================================================================
		UniquePtr<class IRenderPipeline> Create() const override 
		{ 
			return MakeUnique<HighDefinitionRenderPipeline>();
		}

		//=========================================================================================
		RHIFenceValue Render( RenderViewList a_Views ) override;

	};

}