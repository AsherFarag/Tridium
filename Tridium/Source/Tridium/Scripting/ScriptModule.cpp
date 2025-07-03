#include "tripch.h"
#include "ScriptModule.h"

namespace Tridium {

	REGISTER_ENGINE_MODULE( ScriptModule );

	void ScriptModule::Init()
	{
		ScriptEngine::Singleton::Construct();
		if ( !ASSERT( ScriptEngine::Get()->Init(),
					  "Failed to initialize the Script Engine" ) )
		{
			return;
		}
	}

	void ScriptModule::Shutdown()
	{
		ScriptEngine::Get()->Shutdown();
		ScriptEngine::Singleton::Destroy();
	}

}