#pragma once
#include <Tridium/Engine/EngineModule.h>
#include "ScriptEngine.h"

namespace Tridium {

	//================================================================
	// Script Module
	//  Handles the initialization and shutdown of the script engine.
	//  Created as a core engine module and registered with the engine module factory.
	DEFINE_ENGINE_MODULE( ScriptModule, EEngineModuleCategory::Client | EEngineModuleCategory::Server )
	{
	private:
		void Init() override;
		void Shutdown() override;
	};
	//================================================================

}