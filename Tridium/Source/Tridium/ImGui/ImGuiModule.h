#pragma once
#include <Tridium/ImGui/ImGuiBackend.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Engine/EngineModule.h>

namespace Tridium {

	//=================================================================================================
	// ImGui Module: Manages the ImGui Layer and platform/renderer backends.
	//=================================================================================================
	DEFINE_ENGINE_MODULE( ImGuiModule, EEngineModuleCategory::Client, "RendererModule" )
	{
	public:

		//=============================================================================================
		static IPlatformImGuiInterface* GetPlatformBackend() { return Get()->m_PlatformBackend.get(); }
		static const RHICommandListRef& GetCommandList() { return Get()->m_CmdList; }

	private:

		//=============================================================================================
		UniquePtr<IPlatformImGuiInterface> m_PlatformBackend;
		RHICommandListRef m_CmdList;

		void Init() override;
		void Shutdown() override;
		void BeginRender();
		void Render();
		void EndRender();
	};

} // namespace Tridium