#pragma once
#include <Tridium/Engine/EngineModule.h>
#include "PhysicsEngine.h"

namespace Tridium {

	class PhysicsModule : public IEngineModule
	{
	public:
		virtual Optional<EngineModuleError> Init() override
		{
			m_PhysicsEngine = PhysicsEngine::Create();
			if ( !m_PhysicsEngine )
			{
				return EngineModuleError{ EngineModuleError::FatalError, "Failed to create PhysicsEngine" };
			}

			m_PhysicsEngine->Init();
			return {};
		}

		virtual Optional<EngineModuleError> Shutdown() override
		{
			m_PhysicsEngine->Shutdown();
			return {};
		}

		PhysicsEngine& GetPhysicsEngine() { return *m_PhysicsEngine; }

	private:
		UniquePtr<PhysicsEngine> m_PhysicsEngine;
	};

} // namespace Tridium