#pragma once
#include "RHIResource.h"

namespace Tridium {

	//==========================================================
	// RHIBuffer
	//  A buffer resource that stores generic data that can be
	//  written to and read from by both the CPU and GPU.
	//  To create a constant buffer, set the UsageHint to ERHIUsageHint::Default.
	//  For a mutable buffer, set the UsageHint to ERHIUsageHint::MutableBuffer.
	//==========================================================
	DEFINE_RHI_RESOURCE( Buffer )
	{
		Span<const Byte> InitialData = {};
		ERHIUsageHint UsageHint = ERHIUsageHint::Default;
	};

} // namespace Tridium