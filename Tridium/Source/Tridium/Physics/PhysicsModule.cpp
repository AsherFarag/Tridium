#include "tripch.h"
#include "PhysicsModule.h"

namespace Tridium {

	REGISTER_ENGINE_MODULE( PhysicsModule );

	void PhysicsModule::Init()
	{
		m_PhysicsEngine = PhysicsEngine::Create();
		if ( !ASSERT( m_PhysicsEngine,
					  "Failed to create Physics Engine" ) )
		{
			return;
		}

		m_PhysicsEngine->Init();
	}

	void PhysicsModule::Shutdown()
	{
		if ( m_PhysicsEngine )
		{
			m_PhysicsEngine->Shutdown();
			m_PhysicsEngine.reset();
		}
	}

} // namespace Tridium