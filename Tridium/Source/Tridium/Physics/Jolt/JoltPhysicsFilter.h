#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Body/BodyFilter.h>
#include <Jolt/Physics/Body/Body.h>
#include <Tridium/Physics/PhysicsFilter.h>
#include <Tridium/Physics/PhysicsLayer.h>

namespace Tridium {

	class JoltObjectLayerFilter : public JPH::ObjectLayerFilter
	{
	public:
		JoltObjectLayerFilter( ERayCastChannel a_RayCastChannel )
			: RayCastChannel( a_RayCastChannel )
		{}

		/// Filter function. Returns true if we should collide with inObjectLayer
		virtual bool ShouldCollide( JPH::ObjectLayer inObjectLayer ) const override
		{
			TODO( "Implement proper Channel filters" )
			EPhysicsLayer objectLayer = static_cast<EPhysicsLayer>( inObjectLayer );
			switch ( RayCastChannel )
			{
			case ERayCastChannel::Visibility:
				switch ( objectLayer )
				{
				case EPhysicsLayer::Static:
					return true;
				case EPhysicsLayer::Dynamic:
					return true;
				case EPhysicsLayer::Player:
					return true;
				default:
					return true;
				}
			case ERayCastChannel::Camera:
				switch ( objectLayer )
				{
				case EPhysicsLayer::Static:
					return true;
				case EPhysicsLayer::Dynamic:
					return true;
				case EPhysicsLayer::Player:
					return false;
				default:
					return true;
				}
			default:
				return true;
			}
		}

		ERayCastChannel RayCastChannel;
	};

	class JoltBodyFilter : public JPH::BodyFilter
	{
	public:
		JoltBodyFilter() = default;
		JoltBodyFilter( const PhysicsBodyFilter& inBodyFilter )
			: BodyFilter( inBodyFilter )
		{}

		/// Filter function. Returns true if we should collide with inBodyID
		virtual bool ShouldCollide( const JPH::BodyID& inBodyID ) const override
		{
			return !BodyFilter.contains( static_cast<PhysicsBodyID>( inBodyID.GetIndexAndSequenceNumber() ) );
		}

		/// Filter function. Returns true if we should collide with inBody (this is called after the body is locked and makes it possible to filter based on body members)
		virtual bool ShouldCollideLocked( const JPH::Body& inBody ) const override
		{
			return !BodyFilter.contains( static_cast<PhysicsBodyID>( inBody.GetID().GetIndexAndSequenceNumber() ));
		}

		PhysicsBodyFilter BodyFilter;
	};
}