#include "tripch.h"
#include "PhysicsLayer.h"
#include "MotionType.h"

namespace Tridium {
	PhysicsLayerMask::PhysicsLayerMask()
	{
	}
	PhysicsLayerMask::PhysicsLayerMask( const std::bitset<Cast<size_t>( EPhysicsLayer::NUM_LAYERS )>& a_LayerMask )
		: m_LayerMask( a_LayerMask ) {}
} // namespace Tridium
