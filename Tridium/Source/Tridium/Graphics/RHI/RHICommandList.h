#pragma once
#include "RHICommandBuffer.h"

namespace Tridium {

	DEFINE_RHI_RESOURCE( CommandList,
		virtual bool SetGraphicsCommands( const RHIGraphicsCommandBuffer& a_CmdBuffer ) = 0;
		virtual bool SetComputeCommands( const RHIComputeCommandBuffer& a_CmdBuffer ) = 0;
	)
	{
	};

}