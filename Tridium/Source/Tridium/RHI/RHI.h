#pragma once
#include "RHICommon.h"
#include "DynamicRHI.h"

namespace Tridium {

	namespace RHI {

		//===========================
		// Core RHI functions
		bool Initialise( const RHIConfig& a_Config );
		bool Shutdown();
		bool Present();
		//===========================

		template<typename T = DynamicRHI>
		T* GetDynamicRHI() requires Concepts::IsDynamicRHI<T>
		{
			return static_cast<T*>( s_DynamicRHI );
		}
	}

}
