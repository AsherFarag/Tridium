#pragma once
#include "RHICommon.h"
#include "DynamicRHI.h"
#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	class RenderContext final
	{
	public:
		static RenderContext* Get() { return s_Instance; }
		static const RHIConfig& GetConfig() { return s_Instance->m_Config; }
		static ERHInterfaceType GetRHInterfaceType() { return s_Instance->m_Config.RHIType; }

	private:
		RHIConfig m_Config;
		UniquePtr<DynamicRHI> m_RHInterface;

	private: // Singleton
		static RenderContext* s_Instance;
		RenderContext() = default;
		~RenderContext() = default;
		RenderContext( const RenderContext& ) = delete;
		RenderContext& operator=( const RenderContext& ) = delete;

		friend bool RHI::Initialise( const RHIConfig& a_Config );
		friend bool RHI::Shutdown();
		friend bool RHI::Present();
	};
}