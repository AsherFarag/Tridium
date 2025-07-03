#pragma once
#include "ImGuiLayer.h"
#include "ImGuiBackend.h"
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Engine/EngineModule.h>

namespace Tridium {

	DEFINE_ENGINE_MODULE( ImGuiModule, EEngineModuleCategory::Client, "RendererModule" )
	{
	public:
		static IPlatformImGuiInterface* GetPlatformBackend() { return Get()->m_PlatformBackend.get(); }
		static IRendererImGuiInterface* GetRendererBackend() { return Get()->m_RendererBackend.get(); }
		static const RHICommandListRef& GetCommandList() { return Get()->m_CmdList; }
		static ImGuiLayer* GetImGuiLayer() { return Get()->m_ImGuiLayer; }

	private:
		UniquePtr<IPlatformImGuiInterface> m_PlatformBackend;
		UniquePtr<IRendererImGuiInterface> m_RendererBackend;
		RHICommandListRef m_CmdList;
		ImGuiLayer* m_ImGuiLayer = nullptr;

		void Init() override;
		void Shutdown() override;
	};

}