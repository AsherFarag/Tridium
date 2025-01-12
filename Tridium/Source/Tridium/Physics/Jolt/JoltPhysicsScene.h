#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Physics/PhysicsScene.h>
#include <Tridium/Physics/PhysicsLayer.h>
#include <Tridium/Core/Containers/BidirectionalMap.h>
#include <Tridium/ECS/GameObject.h>

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

// STL includes
#include <iostream>
#include <cstdarg>
#include <thread>

namespace Tridium {

	class JoltPhysicsScene final : public PhysicsScene
	{
	public:
		JoltPhysicsScene();
		~JoltPhysicsScene() = default;

		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void Tick( float a_TimeStep ) override;

		virtual GameObject GetGameObjectFromPhysicsBody( PhysicsBodyID a_BodyID ) const override;
		virtual PhysicsBodyID GetPhysicsBodyFromGameObject( GameObject a_GameObject ) const override;

		virtual RayCastResult CastRay( const Vector3& a_Start, const Vector3& a_End, ERayCastChannel a_Channel, const PhysicsBodyFilter& a_BodyFilter ) override;

		virtual void RemovePhysicsBody( PhysicsBodyID a_PhysicsBodyID ) override;
		virtual void RemovePhysicsBody( RigidBodyComponent& a_RigidBody ) override;
		virtual bool AddPhysicsBody( const GameObject& a_GameObject, RigidBodyComponent& a_RigidBody, TransformComponent& a_TransformComponent ) override;
		virtual bool UpdatePhysicsBody( const GameObject& a_GameObject, RigidBodyComponent& a_RigidBody, TransformComponent& a_TransformComponent ) override;

		virtual void UpdatePhysicsBodyTransform( const RigidBodyComponent& a_RigidBody, const TransformComponent& a_TransformComponent ) override;

		virtual Vector3 GetPhysicsBodyPosition( PhysicsBodyID a_BodyID ) const override;
		virtual Quaternion GetPhysicsBodyRotation( PhysicsBodyID a_BodyID ) const override;
		virtual Vector3 GetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID ) const override;
		virtual Vector3 GetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID ) const override;
		virtual bool IsPhysicsBodySleeping( PhysicsBodyID a_BodyID ) const override;

		virtual void SetPhysicsBodyPosition( PhysicsBodyID a_BodyID, const Vector3& a_Position ) override;
		virtual void SetPhysicsBodyRotation( PhysicsBodyID a_BodyID, const Quaternion& a_Rotation ) override;
		virtual void SetPhysicsBodyFriction( PhysicsBodyID a_BodyID, float a_Friction ) override;
		virtual void SetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID, const Vector3& a_LinearVelocity ) override;
		virtual void SetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID, const Vector3& a_AngularVelocity ) override;
		virtual void AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse ) override;
		virtual void AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse, const Vector3& a_Position ) override;

	#if USE_DEBUG_RENDERER
		virtual void RenderDebug( const Matrix4& a_ViewProjection ) override;
	#endif

	protected:
		bool m_Initialised = false;

		PhysicsLayerManager m_LayerManager;
		JPH::PhysicsSystem m_PhysicsSystem;
		JPH::BodyInterface& m_BodyInterface;

		UniquePtr<JPH::JobSystemThreadPool> m_JobSystem;
		UniquePtr<JPH::TempAllocatorImpl> m_TempAllocator;
		UniquePtr<JPH::BroadPhaseLayerInterface> m_BroadPhaseLayerInterface;
		UniquePtr<JPH::ObjectVsBroadPhaseLayerFilter> m_ObjectVsBroadPhaseLayerFilter;
		UniquePtr<JPH::ObjectLayerPairFilter> m_ObjectLayerPairFilter;

		BidirectionalMap<PhysicsBodyID, EntityID> m_BodyToGameObjectMap;

		struct
		{
			const uint32_t MaxBodies = 65536;
			const uint32_t NumBodyMutexes = 0;
			const uint32_t MaxBodyPairs = 65536;
			const uint32_t MaxContactConstraints = 10240;
			const uint32_t CollisionSteps = 1;
		} m_PhysicsSystemSettings;

		uint64_t m_CurrentStep = 0;
	};

}