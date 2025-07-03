#pragma once
#include <Tridium/Engine/EngineModule.h>
#include "PhysicsEngine.h"

namespace Tridium {

	//================================================================
	// Physics Module
	//  Handles the initialization and shutdown of the physics engine.
	//  Created as a core engine module and registered with the engine module factory.
	DEFINE_ENGINE_MODULE( PhysicsModule, EEngineModuleCategory::Server | EEngineModuleCategory::Client, "ScriptModule", "RendererModule" )
	{
	public:
		PhysicsEngine& GetPhysicsEngine() { return *m_PhysicsEngine; }

	private:
		UniquePtr<PhysicsEngine> m_PhysicsEngine;

	private:
		void Init() override;
		void Shutdown() override;
	};
	//================================================================

} // namespace Tridium