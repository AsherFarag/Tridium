#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Physics/MotionType.h>
#include <Tridium/Physics/PhysicsBodyProxy.h>

namespace Tridium {

	class RigidBodyComponent : public Component
	{
		REFLECT( RigidBodyComponent )
	public:
		RigidBodyComponent() = default;
		RigidBodyComponent( const RigidBodyComponent& a_Other ) = default;
		RigidBodyComponent( RigidBodyComponent&& a_Other ) noexcept = default;
		RigidBodyComponent& operator=( const RigidBodyComponent& a_Other ) = default;

		bool IsSleeping() const { return m_BodyProxy.IsSleeping(); }

		EPhysicsLayer GetPhysicsLayer() const { return m_PhysicsLayer; }
		EMotionType GetMotionType() const { return m_MotionType; }
		Vector3 GetLinearVelocity() const { return m_BodyProxy.GetLinearVelocity(); }
		Vector3 GetAngularVelocity() const { return m_BodyProxy.GetAngularVelocity(); }
		float GetGravityScale() const { return m_GravityScale; }
		float GetRestitution() const { return m_Restitution; }
		float GetMass() const { return m_Mass; }

		void SetPhysicsLayer( EPhysicsLayer a_PhysicsLayer ) { m_PhysicsLayer = a_PhysicsLayer; }
		void SetMotionType( EMotionType a_MotionType ) { m_MotionType = a_MotionType; }
		void SetMass( float a_Mass ) { m_Mass = a_Mass; }
		void SetGravityScale( float a_GravityScale ) { m_GravityScale = a_GravityScale; }
		void SetRestitution( float a_Restitution ) { m_Restitution = a_Restitution; }
		void SetFriction( float a_Friction ) { m_BodyProxy.SetFriction( a_Friction ); }
		void SetLinearVelocity( const Vector3& a_LinearVelocity ) { m_BodyProxy.SetLinearVelocity( a_LinearVelocity ); }
		void SetAngularVelocity( const Vector3& a_AngularVelocity ) { m_BodyProxy.SetAngularVelocity( a_AngularVelocity ); }
		void AddImpulse( const Vector3& a_Impulse ) { m_BodyProxy.AddImpulse( a_Impulse ); }
		void AddImpulse( const Vector3& a_Impulse, const Vector3& a_Position ) { m_BodyProxy.AddImpulse( a_Impulse, a_Position ); }

		const LinearMotionConstraint& GetLinearMotionConstraint() const { return m_LinearMotionConstraint; }
		const AngularMotionConstraint& GetAngularMotionConstraint() const { return m_AngularMotionConstraint; }

		PhysicsBodyProxy& GetBodyProxy() { return m_BodyProxy; }
		const PhysicsBodyProxy& GetBodyProxy() const { return m_BodyProxy; }

	protected:
		PhysicsBodyProxy m_BodyProxy;
		EPhysicsLayer m_PhysicsLayer = EPhysicsLayer::Dynamic;
		EMotionType m_MotionType = EMotionType::Dynamic;
		float m_Mass = 1.0f;
		float m_GravityScale = 1.0f;
		float m_Restitution = 0.5f;

		LinearMotionConstraint m_LinearMotionConstraint;
		AngularMotionConstraint m_AngularMotionConstraint;

		friend class Scene;
	};

	BEGIN_REFLECT_COMPONENT( RigidBodyComponent )
		PROPERTY( m_BodyProxy )
		PROPERTY( m_PhysicsLayer, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( m_MotionType, FLAGS(EditAnywhere, Serialize) )
		PROPERTY( m_Mass, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( m_GravityScale, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( m_Restitution, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( m_LinearMotionConstraint, FLAGS( EditAnywhere, Serialize ) )
		PROPERTY( m_AngularMotionConstraint, FLAGS( EditAnywhere, Serialize ) )
	END_REFLECT( RigidBodyComponent )

}