#pragma once
#include <unordered_set>
#include "PhysicsBody.h"

namespace Tridium {

	// Forward declarations
	class OldRigidBodyComponent;
	// -------------------

	class PhysicsBodyFilter : public std::unordered_set<PhysicsBodyID>
	{
	public:
		PhysicsBodyFilter() = default;
		PhysicsBodyFilter( const PhysicsBodyID& a_BodyID )
		{
			insert( a_BodyID );
		}

		PhysicsBodyFilter( const std::initializer_list<PhysicsBodyID>& a_BodyIDs )
			: std::unordered_set<PhysicsBodyID>( a_BodyIDs )
		{
		}

		PhysicsBodyFilter( const OldRigidBodyComponent& a_RigidBodyComponent );
		PhysicsBodyFilter( const std::initializer_list<OldRigidBodyComponent>& a_RigidBodyComponents );
	};

}