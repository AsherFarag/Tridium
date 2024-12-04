#include "tripch.h"
#include "PhysicsScene.h"
#include "PhysicsAPI.h"
#include "Jolt/JoltPhysicsScene.h"
#include <Tridium/Reflection/Reflection.h>

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
		ENUM_VALUE( Locked )
		ENUM_VALUE( Limited )
		ENUM_VALUE( Free )
	END_REFLECT_ENUM( ESixDOFConstraintMotion );

	BEGIN_REFLECT( LinearMotionConstraint )
		PROPERTY( XMotion, EditAnywhere | Serialize )
		PROPERTY( XLimit,  EditAnywhere | Serialize )
		PROPERTY( YMotion, EditAnywhere | Serialize )
		PROPERTY( YLimit,  EditAnywhere | Serialize )
		PROPERTY( ZMotion, EditAnywhere | Serialize )
		PROPERTY( ZLimit,  EditAnywhere | Serialize )
	END_REFLECT( LinearMotionConstraint );

	BEGIN_REFLECT( AngularMotionConstraint )
		PROPERTY( Swing1Motion, EditAnywhere | Serialize )
		PROPERTY( Swing1Limit,  EditAnywhere | Serialize )
		PROPERTY( Swing2Motion, EditAnywhere | Serialize )
		PROPERTY( Swing2Limit,  EditAnywhere | Serialize )
		PROPERTY( TwistMotion,  EditAnywhere | Serialize )
		PROPERTY( TwistLimit,   EditAnywhere | Serialize )
	END_REFLECT( AngularMotionConstraint );

}
