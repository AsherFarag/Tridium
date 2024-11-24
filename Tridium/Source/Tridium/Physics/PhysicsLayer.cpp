#include "tripch.h"
#include "PhysicsLayer.h"
#include <Tridium/Utils/Reflection/Reflection.h>

namespace Tridium {
	PhysicsLayerMask::PhysicsLayerMask()
	{
	}
	PhysicsLayerMask::PhysicsLayerMask( const std::bitset<static_cast<size_t>( EPhysicsLayer::NUM_LAYERS )>& a_LayerMask )
		: m_LayerMask( a_LayerMask ) {}

	BEGIN_REFLECT_ENUM( EPhysicsLayer )
		ENUM_VAL( Static )
		ENUM_VAL( Dynamic )
		ENUM_VAL( Player )
	END_REFLECT_ENUM( EPhysicsLayer );

	BEGIN_REFLECT_ENUM( ERayCastChannel )
		ENUM_VAL( Visibility )
		ENUM_VAL( Camera )
	END_REFLECT_ENUM( ERayCastChannel );

	BEGIN_REFLECT_ENUM( ECollisionResponse )
		ENUM_VAL( Ignore )
		ENUM_VAL( Overlap )
		ENUM_VAL( Block )
	END_REFLECT_ENUM( ECollisionResponse );
} // namespace Tridium
