#include "tripch.h"
#include "PhysicsFilter.h"
#include <Tridium/ECS/Components/Types/Physics/RigidBodyComponent.h>

namespace Tridium {

	PhysicsBodyFilter::PhysicsBodyFilter( const RigidBodyComponent& a_RigidBodyComponent )
		: std::unordered_set<PhysicsBodyID>()
	{
		insert( a_RigidBodyComponent.GetBodyProxy().GetBodyID() );
	}

	PhysicsBodyFilter::PhysicsBodyFilter( const std::initializer_list<RigidBodyComponent>& a_RigidBodyComponents )
		: std::unordered_set<PhysicsBodyID>()
	{
		for ( const RigidBodyComponent& rigidBody : a_RigidBodyComponents )
		{
			insert( rigidBody.GetBodyProxy().GetBodyID() );
		}
	}

} // namespace Tridium
