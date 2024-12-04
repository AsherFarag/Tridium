#include "tripch.h"
#include "PhysicsLayer.h"
#include "MotionType.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {
	PhysicsLayerMask::PhysicsLayerMask()
	{
	}
	PhysicsLayerMask::PhysicsLayerMask( const std::bitset<static_cast<size_t>( EPhysicsLayer::NUM_LAYERS )>& a_LayerMask )
		: m_LayerMask( a_LayerMask ) {}


	TODO( "We only are reflecting here as the MotionType.cpp doesn't not get linked by the compiler. This is a bug in the build system." )
	BEGIN_REFLECT_ENUM( EMotionType )
		ENUM_VALUE( Static )
		ENUM_VALUE( Kinematic )
		ENUM_VALUE( Dynamic )
	END_REFLECT_ENUM( EMotionType )

	BEGIN_REFLECT_ENUM( EPhysicsLayer )
		ENUM_VALUE( Static )
		ENUM_VALUE( Dynamic )
		ENUM_VALUE( Player )
	END_REFLECT_ENUM( EPhysicsLayer );

	BEGIN_REFLECT_ENUM( ERayCastChannel )
		ENUM_VALUE( Visibility )
		ENUM_VALUE( Camera )
	END_REFLECT_ENUM( ERayCastChannel );

	BEGIN_REFLECT_ENUM( ECollisionResponse )
		ENUM_VALUE( Ignore )
		ENUM_VALUE( Overlap )
		ENUM_VALUE( Block )
	END_REFLECT_ENUM( ECollisionResponse );
} // namespace Tridium
