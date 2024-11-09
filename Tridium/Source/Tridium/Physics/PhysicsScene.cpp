#include "tripch.h"
#include "PhysicsScene.h"
#include "Jolt/JoltPhysicsScene.h"

namespace Tridium {
    UniquePtr<PhysicsScene> PhysicsScene::Create()
    {
        return MakeUnique<JoltPhysicsScene>();
    }
}
