#pragma once
#include <Tridium/Physics/PhysicsBody.h>
#include <Tridium/Containers/UnorderedSet.h>

namespace Tridium {

	class PhysicsBodyFilter : public UnorderedSet<PhysicsBodyID>
	{
	public:
		PhysicsBodyFilter() = default;
		PhysicsBodyFilter( const PhysicsBodyID& a_BodyID )
		{
			insert( a_BodyID );
		}

		PhysicsBodyFilter( const std::initializer_list<PhysicsBodyID>& a_BodyIDs )
			: UnorderedSet<PhysicsBodyID>( a_BodyIDs )
		{
		}
	};

}