#pragma once
#include "RenderResourceManager.h"
#include <Tridium/Engine/EngineModule.h>

namespace Tridium {

	class RendererModule : IEngineModule
	{
	public:

		auto& GetRenderResourceManager() { return m_RenderResourceManager; }

	private:
		RenderResourceManager m_RenderResourceManager;
	};

}