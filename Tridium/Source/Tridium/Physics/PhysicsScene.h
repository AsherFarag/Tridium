#pragma once
#include <Tridium/Core/Memory.h>

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

		virtual void RemovePhysicsBody( RigidBodyComponent& a_RigidBody ) = 0;
		virtual bool AddPhysicsBody( const GameObject& a_GameObject, RigidBodyComponent& a_RigidBody, TransformComponent& a_TransformComponent ) = 0;
		virtual bool UpdatePhysicsBody( const GameObject& a_GameObject, RigidBodyComponent& a_RigidBody, TransformComponent& a_TransformComponent ) = 0;

		virtual void UpdatePhysicsBodyTransform( const RigidBodyComponent& a_RigidBody, const TransformComponent& a_TransformComponent ) = 0;

	protected:
		Scene* m_Scene;
		friend Scene;
	};

} // namespace Tridium