#pragma once
#include <Tridium/Core/Types.h>
#include <array>
#include <bitset>
#include "PhysicsBody.h"

namespace Tridium {

	enum class EPhysicsLayer : uint8_t
	{
		Static = 0, // A rigid body in this layer should have it's MotionType set to Static. Dynamic objects in a static layer can harm performance.
		Dynamic,
		Player,
		NUM_LAYERS,
		INVALID = 0xFF
	};

	enum class ERayCastChannel : uint8_t
	{
		Visibility = 0,
		Camera,
		NUM_CHANNELS,
		INVALID = 0xFF
	};

	enum class ECollisionResponse : uint8_t
	{
		Ignore = 0,
		Overlap, // NOT IMPLEMENTED
		Block
	};

	class PhysicsLayerMask
	{
	public:
		PhysicsLayerMask();
		PhysicsLayerMask( const std::bitset<static_cast<size_t>( EPhysicsLayer::NUM_LAYERS )>& a_LayerMask );

		void SetCollisionResponse( EPhysicsLayer a_Layer, ECollisionResponse a_Response )
		{
			m_LayerMask.set( static_cast<size_t>( a_Layer ), a_Response != ECollisionResponse::Ignore );
		}

		ECollisionResponse GetCollisionResponse( EPhysicsLayer a_Layer ) const
		{
			return m_LayerMask.test( static_cast<size_t>( a_Layer ) ) ? ECollisionResponse::Block : ECollisionResponse::Ignore;
		}

	private:
		std::bitset<static_cast<size_t>( EPhysicsLayer::NUM_LAYERS )> m_LayerMask;
	};

	class PhysicsLayerManager
	{
	public:
		const PhysicsLayerMask& GetLayerMask( EPhysicsLayer a_Layer ) const
		{
			return m_LayerMasks.at( static_cast<size_t>( a_Layer ) );
		}

		void SetLayerMask( EPhysicsLayer a_Layer, const PhysicsLayerMask& a_Mask )
		{
			m_LayerMasks[static_cast<size_t>( a_Layer )] = a_Mask;
		}

		ECollisionResponse GetCollisionResponse( EPhysicsLayer a_Layer, EPhysicsLayer a_OtherLayer ) const
		{
			return m_LayerMasks[static_cast<size_t>( a_Layer )].GetCollisionResponse( a_OtherLayer );
		}
		 
	private:
		std::array<PhysicsLayerMask, static_cast<size_t>(EPhysicsLayer::NUM_LAYERS)> m_LayerMasks;
	};

} // namespace Tridium