#pragma once
#include <Tridium/Core/Memory.h>
#include <Tridium/Math/Math.h>

namespace Tridium {

	// Forward Declarations
	class Scene;
	class RigidBodyComponent;
	class TransformComponent;
	class GameObject;
	// --------------------

	using PhysicsBodyID = uint32_t;
	constexpr PhysicsBodyID INVALID_PHYSICS_BODY_ID = 0xffffffff;

	class PhysicsScene
	{
	public:
		static UniquePtr<PhysicsScene> Create();

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick( float a_TimeStep ) = 0;

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
		virtual void SetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID, const Vector3& a_LinearVelocity ) = 0;
		virtual void SetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID, const Vector3& a_AngularVelocity ) = 0;

	protected:
		Scene* m_Scene;
		friend Scene;
	};

} // namespace Tridium