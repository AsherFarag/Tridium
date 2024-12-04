#pragma once
#include <Tridium/Core/Memory.h>
#include <Tridium/Math/Math.h>

#include "PhysicsBody.h"
#include "PhysicsLayer.h"
#include "PhysicsFilter.h"
#include "RayCast.h"
#include <Tridium/Reflection/ReflectionFwd.h>

#if TE_SHIPPING
	#define USE_DEBUG_RENDERER 0
#else
	#define USE_DEBUG_RENDERER 1
#endif

namespace Tridium {

	// Forward Declarations
	class Scene;
	class RigidBodyComponent;
	class TransformComponent;
	class GameObject;
	// --------------------

	enum class ESixDOFConstraintMotion : uint8_t
	{
		Locked,
		Limited,
		Free
	};

	struct LinearMotionConstraint
	{
		REFLECT( LinearMotionConstraint );

		ESixDOFConstraintMotion XMotion = ESixDOFConstraintMotion::Free;
		ESixDOFConstraintMotion YMotion = ESixDOFConstraintMotion::Free;
		ESixDOFConstraintMotion ZMotion = ESixDOFConstraintMotion::Free;

		float XLimit = 0.0f;
		float YLimit = 0.0f;
		float ZLimit = 0.0f;
	};

	struct AngularMotionConstraint
	{
		REFLECT( AngularMotionConstraint );

		ESixDOFConstraintMotion Swing1Motion = ESixDOFConstraintMotion::Free;
		ESixDOFConstraintMotion Swing2Motion = ESixDOFConstraintMotion::Free;
		ESixDOFConstraintMotion TwistMotion = ESixDOFConstraintMotion::Free;

		float Swing1Limit = 0.0f;
		float Swing2Limit = 0.0f;
		float TwistLimit = 0.0f;
	};

	class PhysicsScene
	{
	public:
		static UniquePtr<PhysicsScene> Create();

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick( float a_TimeStep ) = 0;

		virtual RayCastResult CastRay( const Vector3& a_Start, const Vector3& a_End, ERayCastChannel a_Channel = ERayCastChannel::Visibility, const PhysicsBodyFilter& a_BodyFilter = {} ) = 0;

		virtual void RemovePhysicsBody( PhysicsBodyID a_PhysicsBodyID ) = 0;
		virtual void RemovePhysicsBody( RigidBodyComponent& a_RigidBody ) = 0;
		virtual bool AddPhysicsBody( const GameObject& a_GameObject, RigidBodyComponent& a_RigidBody, TransformComponent& a_TransformComponent ) = 0;
		virtual bool UpdatePhysicsBody( const GameObject& a_GameObject, RigidBodyComponent& a_RigidBody, TransformComponent& a_TransformComponent ) = 0;

		virtual void UpdatePhysicsBodyTransform( const RigidBodyComponent& a_RigidBody, const TransformComponent& a_TransformComponent ) = 0;

		virtual Vector3 GetPhysicsBodyPosition( PhysicsBodyID a_BodyID ) const = 0;
		virtual Quaternion GetPhysicsBodyRotation( PhysicsBodyID a_BodyID ) const = 0;
		virtual Vector3 GetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID ) const = 0;
		virtual Vector3 GetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID ) const = 0;
		virtual bool IsPhysicsBodySleeping( PhysicsBodyID a_BodyID ) const = 0;

		virtual void SetPhysicsBodyPosition( PhysicsBodyID a_BodyID, const Vector3& a_Position ) = 0;
		virtual void SetPhysicsBodyRotation( PhysicsBodyID a_BodyID, const Quaternion& a_Rotation ) = 0;
		virtual void SetPhysicsBodyFriction( PhysicsBodyID a_BodyID, float a_Friciton ) = 0;
		virtual void SetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID, const Vector3& a_LinearVelocity ) = 0;
		virtual void SetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID, const Vector3& a_AngularVelocity ) = 0;
		virtual void AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse ) = 0;
		virtual void AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse, const Vector3& a_Position ) = 0;

	#if USE_DEBUG_RENDERER
		virtual void RenderDebug( const Matrix4& a_ViewProjection ) = 0;
	#endif

	protected:
		Scene* m_Scene;
		friend Scene;
	};

} // namespace Tridium