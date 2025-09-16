#pragma once
#include <Tridium/Core/Application.h>
#include <Tridium/Engine/EngineModule.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/RHI/DynamicRHI.h>

namespace Tridium {

	DEFINE_TICK_GROUP( BeginRender );
	DEFINE_TICK_GROUP( EndRender );

	//=============================================================================================
	// Renderer Module:
	//=============================================================================================
	DEFINE_ENGINE_MODULE( RendererModule, EEngineModuleCategory::Server | EEngineModuleCategory::Client )
	{
	public:

		//=========================================================================================
		IDynamicRHI* DynamicRHI() { return m_DynamicRHI; }

		auto& ResourceManager() { return m_RenderResourceManager; }
		const auto& ResourceManager() const { return m_RenderResourceManager; }

	private:

		//=========================================================================================
		IDynamicRHI* m_DynamicRHI;
		RenderResourceManager m_RenderResourceManager;

		//=========================================================================================
		void Init() override;
		void Shutdown() override;

	};

}