#pragma once
#include <Tridium/Core/Application.h>
#include <Tridium/Engine/EngineModule.h>
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

	private:

		//=========================================================================================
		IDynamicRHI* m_DynamicRHI;

		//=========================================================================================
		void Init() override;
		void Shutdown() override;

	};

}