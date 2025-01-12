#pragma once
#include <Tridium/Core/Engine/EngineSubsystem.h>
#include "PhysicsEngine.h"

namespace Tridium {

	class PhysicsSubsystem : public IEngineSubsystem
	{
	public:
		virtual SubsystemStatus Initialize() override
		{
			m_PhysicsEngine = PhysicsEngine::Create();
			if ( !m_PhysicsEngine )
			{
				return { ESubsystemStatusType::FatalError, "Failed to create PhysicsEngine" };
			}

			m_PhysicsEngine->Init();
			return {};
		}

		virtual const char* GetName() const override { return "PhysicsSubsystem"; }

		PhysicsEngine& GetPhysicsEngine() { return *m_PhysicsEngine; }

	private:
		UniquePtr<PhysicsEngine> m_PhysicsEngine;
	};

} // namespace Tridium