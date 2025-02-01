#pragma once
#include "RHICommon.h"

namespace Tridium {

	//==============================================
	// DynamicRHI
	//  An interace for the dynamicly bound RHI.
	//==============================================
	class DynamicRHI
	{
	public:
		DynamicRHI() = default;
		virtual ~DynamicRHI() = default;

		virtual bool Init( const RHIConfig& a_Config ) = 0;
		virtual bool Shutdown() = 0;
		virtual bool Present() = 0;

	#if RHI_DEBUG_ENABLED
		virtual void DumpDebug() {}
	#endif // RHI_DEBUG_ENABLED
	};

}