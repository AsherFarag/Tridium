#pragma once
#include <Tridium/Engine/EngineModule.h>
#include "ScriptEngine.h"

namespace Tridium {

	//================================================================
	// Script Module
	//  Handles the initialization and shutdown of the script engine.
	//  Created as a core engine module and registered with the engine module factory.
	class ScriptModule final : public IEngineModule
	{
	public:
		virtual Optional<EngineModuleError> Init() override
		{
			ScriptEngine::Singleton::Construct();
			if ( !ScriptEngine::Get()->Init() )
			{
				return EngineModuleError( EngineModuleError::ELevel::FatalError, "Failed to initialize the script engine" );
			}

			return {};
		}

		virtual Optional<EngineModuleError> Shutdown() override
		{
			ScriptEngine::Get()->Shutdown();
			ScriptEngine::Singleton::Destroy();
			return {};
		}
	};
	//================================================================

}