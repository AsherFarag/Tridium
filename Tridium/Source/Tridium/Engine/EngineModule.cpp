#include "tripch.h"
#include "EngineModule.h"

// Core Modules
#include <Tridium/Physics/PhysicsModule.h>
#include <Tridium/Scripting/ScriptModule.h>

namespace Tridium {

	void EngineModuleFactory::RegisterCoreModules()
	{
		RegisterModule<PhysicsModule>( "PhysicsModule", EEngineInitStage::PreEngineInit, EEngineModuleCategory::Physics );
		RegisterModule<ScriptModule>( "ScriptModule", EEngineInitStage::PreEngineInit, EEngineModuleCategory::Scripting );
	}

} // namespace Tridium
