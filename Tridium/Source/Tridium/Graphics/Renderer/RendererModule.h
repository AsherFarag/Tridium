#pragma once
#include "RenderResourceManager.h"
#include "ShaderLibrary.h"
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Engine/EngineModule.h>

namespace Tridium {

	DEFINE_ENGINE_MODULE( RendererModule, EEngineModuleCategory::Server | EEngineModuleCategory::Client )
	{
	public:
		IDynamicRHI* DynamicRHI() { return m_DynamicRHI; }

		auto& ResourceManager() { return m_RenderResourceManager; }
		const auto& ResourceManager() const { return m_RenderResourceManager; }

		auto& ShaderLibrary() { return m_ShaderLibrary; }
		const auto& ShaderLibrary() const { return m_ShaderLibrary; }

		static void Print();

	private:
		IDynamicRHI* m_DynamicRHI;
		RenderResourceManager m_RenderResourceManager;
		class ShaderLibrary m_ShaderLibrary;

		void Init() override;
		void Shutdown() override;
	};

}