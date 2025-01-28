#pragma once
#include "RHICommon.h"
#include "RendererAPI.h"
#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	class RenderContext final
	{
	public:
		static RenderContext* Get() { return s_Instance; }
		static const RHIConfig& GetConfig() { return s_Instance->m_Config; }
		static EGraphicsAPI GetGraphicsAPI() { return s_Instance->m_Config.GraphicsAPI; }

	private:
		RHIConfig m_Config;
		UniquePtr<RendererAPI> m_RendererAPI;

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