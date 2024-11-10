#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Physics/MotionType.h>

namespace Tridium {

	class RigidBodyComponent : public Component
	{
		REFLECT( RigidBodyComponent )
	public:

		void SetMotionType( EMotionType a_MotionType ) { m_MotionType = a_MotionType; }
		EMotionType GetMotionType() const { return m_MotionType; }

		void SetMass( float a_Mass ) { m_Mass = a_Mass; }
		float GetMass() const { return m_Mass; }

		void SetGravityScale( float a_GravityScale ) { m_GravityScale = a_GravityScale; }
		float GetGravityScale() const { return m_GravityScale; }

		// TEMP!
		PhysicsBodyID BodyID = INVALID_PHYSICS_BODY_ID;

	protected:
		EMotionType m_MotionType = EMotionType::Dynamic;
		float m_Mass = 1.0f;
		float m_GravityScale = 1.0f;
	};

	BEGIN_REFLECT_COMPONENT( RigidBodyComponent )
		PROPERTY( m_MotionType, FLAGS(EditAnywhere, Serialize) )
		PROPERTY( m_Mass, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( m_GravityScale, FLAGS( EditAnywhere, Serialize ) )
	END_REFLECT( RigidBodyComponent )

}