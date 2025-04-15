#pragma once
#include "RHICommandBuffer.h"

namespace Tridium {

	//======================================================================================================
	// RHI Command List
	//  Command lists are used to submit work to the GPU.
	//======================================================================================================

	DECLARE_RHI_RESOURCE_DESCRIPTOR( RHICommandListDescriptor, RHICommandList )
	{
	};

	DECLARE_RHI_RESOURCE_INTERFACE( RHICommandList )
	{
		RHI_RESOURCE_INTERFACE_BODY( RHICommandList, ERHIResourceType::CommandList );

		virtual bool Commit( const RHICommandListDescriptor& a_Desc ) = 0;
		virtual bool SetGraphicsCommands( const RHIGraphicsCommandBuffer & a_CmdBuffer ) = 0;
		virtual bool SetComputeCommands( const RHIComputeCommandBuffer & a_CmdBuffer ) = 0;
	};

}