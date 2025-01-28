#pragma once
#include "RHICommon.h"

namespace Tridium {

	class RendererAPI
	{
	public:
		RendererAPI() = default;
		virtual ~RendererAPI() = default;

		virtual bool Init( const RHIConfig& a_Config ) = 0;
		virtual bool Shutdown() = 0;
		virtual bool Present() = 0;

	#if RHI_DEBUG_ENABLED
		virtual void DumpDebug() {}
	#endif // RHI_DEBUG_ENABLED
	};

}