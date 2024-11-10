#include "tripch.h"
#include "PhysicsScene.h"
#include "PhysicsAPI.h"
#include "Jolt/JoltPhysicsScene.h"

namespace Tridium {
    UniquePtr<PhysicsScene> PhysicsScene::Create()
    {
        switch ( s_PhysicsAPI )
        {
		case EPhysicsAPI::Jolt:
			return MakeUnique<JoltPhysicsScene>();
        }

		TE_CORE_ASSERT( false, "Unknown Physics API" );
		return nullptr;
    }
}
