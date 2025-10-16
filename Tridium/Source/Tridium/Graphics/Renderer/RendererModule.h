#pragma once
#include <Tridium/Core/Application.h>
#include <Tridium/Engine/EngineModule.h>
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Graphics/Renderer/RenderPipeline.h>

namespace Tridium {

	DEFINE_TICK_GROUP( BeginRender );
	DEFINE_TICK_GROUP( Render );
	DEFINE_TICK_GROUP( EndRender );

	//=============================================================================================
	// Renderer Module:
	//=============================================================================================
	DEFINE_ENGINE_MODULE( RendererModule, EEngineModuleCategory::Server | EEngineModuleCategory::Client )
	{
	public:

		//=========================================================================================
		IDynamicRHI* DynamicRHI() { return m_DynamicRHI; }

		//=========================================================================================
		static uint32_t GetFrameIndex() { return Valid() ? Get()->m_FrameIndex : 0; }

		//=========================================================================================
		static RenderPipelineManager* GetPipelineManager() { return Valid() ? &Get()->m_PipelineManager : nullptr; }

	private:

		//=========================================================================================
		IDynamicRHI* m_DynamicRHI;
		uint32_t m_FrameIndex = 0;
		RenderPipelineManager m_PipelineManager;

		//=========================================================================================
		void Init() override;
		void Shutdown() override;

		void BeginFrame();
		void EndFrame();

	};

}