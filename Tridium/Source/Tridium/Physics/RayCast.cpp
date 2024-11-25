#include "tripch.h"
#include "RayCast.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types/Physics/RigidBodyComponent.h>

namespace Tridium {

    GameObject RayCastResult::GetHitGameObject() const
    {
		GameObject hitObject;
		auto view = Application::GetScene()->GetRegistry().view<RigidBodyComponent>();
		for ( auto entity : view )
		{
			auto& rigidBody = view.get<RigidBodyComponent>( entity );
			if ( rigidBody.GetBodyProxy().GetBodyID() == HitBodyID )
			{
				hitObject = entity;
				break;
			}
		}

		return hitObject;
    }

    RigidBodyComponent* RayCastResult::GetHitRigidBody() const
    {
		return GetHitGameObject().TryGetComponent<RigidBodyComponent>();
    }

}
