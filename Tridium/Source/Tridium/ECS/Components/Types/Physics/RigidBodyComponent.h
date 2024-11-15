#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Physics/MotionType.h>
#include <Tridium/Physics/PhysicsBody.h>

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

		Vector3 GetLinearVelocity() const { return m_BodyProxy.GetLinearVelocity(); }
		Vector3 GetAngularVelocity() const { return m_BodyProxy.GetAngularVelocity(); }
		bool IsSleeping() const { return m_BodyProxy.IsSleeping(); }

		void SetLinearVelocity( const Vector3& a_LinearVelocity ) { m_BodyProxy.SetLinearVelocity( a_LinearVelocity ); }
		void SetAngularVelocity( const Vector3& a_AngularVelocity ) { m_BodyProxy.SetAngularVelocity( a_AngularVelocity ); }

		PhysicsBodyProxy& GetBodyProxy() { return m_BodyProxy; }
		const PhysicsBodyProxy& GetBodyProxy() const { return m_BodyProxy; }

	protected:
		PhysicsBodyProxy m_BodyProxy;

		EMotionType m_MotionType = EMotionType::Dynamic;
		float m_Mass = 1.0f;
		float m_GravityScale = 1.0f;

		friend class Scene;
	};

	BEGIN_REFLECT_COMPONENT( RigidBodyComponent )
		PROPERTY( m_BodyProxy )
		PROPERTY( m_MotionType, FLAGS(EditAnywhere, Serialize) )
		PROPERTY( m_Mass, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( m_GravityScale, FLAGS( EditAnywhere, Serialize ) )
	END_REFLECT( RigidBodyComponent )

}