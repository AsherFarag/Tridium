#include "tripch.h"
#include "PhysicsEngine.h"
#include "Jolt/JoltPhysicsEngine.h"

namespace Tridium {
    UniquePtr<PhysicsEngine> PhysicsEngine::Create()
    {
		switch ( s_PhysicsAPI )
		{
		case EPhysicsAPI::Jolt:
			return MakeUnique<JoltPhysicsEngine>();
		}

		ASSERT( false, "Unknown physics API" );
		return nullptr;
    }
}
