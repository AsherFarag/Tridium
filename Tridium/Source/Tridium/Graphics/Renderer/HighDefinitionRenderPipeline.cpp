#include "tripch.h"
#include "HighDefinitionRenderPipeline.h"

namespace Tridium {

	RHIFenceValue HighDefinitionRenderPipeline::Render( RenderViewList a_Views )
	{
		for ( const auto& [view, buffer] : a_Views.Cameras() )
		{

		}

		return {};
	}

}