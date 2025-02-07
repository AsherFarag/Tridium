#include "tripch.h"
#include "EngineModule.h"

#include <Tridium/Physics/PhysicsModule.h>

namespace Tridium {

	void EngineModuleFactory::RegisterCoreModules()
	{
		RegisterModule<PhysicsModule>( "PhysicsSubsystem", EEngineInitStage::PreEngineInit, EEngineModuleCategory::Physics );
	}

} // namespace Tridium
