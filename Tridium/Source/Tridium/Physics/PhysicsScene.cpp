#include "tripch.h"
#include "PhysicsScene.h"
#include "PhysicsAPI.h"
#include "Jolt/JoltPhysicsScene.h"
#include <Tridium/Utils/Reflection/Reflection.h>

namespace Tridium {
    UniquePtr<PhysicsScene> PhysicsScene::Create()
    {
        switch ( s_PhysicsAPI )
        {
		case EPhysicsAPI::Jolt:
			return MakeUnique<JoltPhysicsScene>();
        }

		TE_CORE_ASSERT( false, "Unknown Physics API" );
		return nullptr;
    }

	BEGIN_REFLECT_ENUM( ESixDOFConstraintMotion )
		ENUM_VAL( Locked )
		ENUM_VAL( Limited )
		ENUM_VAL( Free )
	END_REFLECT_ENUM( ESixDOFConstraintMotion );

	BEGIN_REFLECT( LinearMotionConstraint )
		PROPERTY( XMotion, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( XLimit, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( YMotion, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( YLimit, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( ZMotion, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( ZLimit, FLAGS( EditAnywhere, Serialize ) )
	END_REFLECT( LinearMotionConstraint );

	BEGIN_REFLECT( AngularMotionConstraint )
		PROPERTY( Swing1Motion, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( Swing1Limit, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( Swing2Motion, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( Swing2Limit, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( TwistMotion, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( TwistLimit, FLAGS( EditAnywhere, Serialize ) )
	END_REFLECT( AngularMotionConstraint );

}
