#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Physics/PhysicsScene.h>
#include <Tridium/Physics/PhysicsLayer.h>
#include <Tridium/Containers/BidirectionalMap.h>

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
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>

#include <iostream>
#include <cstdarg>
#include <thread>

namespace Tridium {

	class JoltPhysicsScene final : public IPhysicsScene
	{
	public:
		JoltPhysicsScene();
		~JoltPhysicsScene() = default;

		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void Tick( float a_TimeStep ) override;

		virtual RayCastResult CastRay( const Vector3& a_Start, const Vector3& a_End, ERayCastChannel a_Channel, const PhysicsBodyFilter& a_BodyFilter ) override;

		PhysicsBodyID CreatePhysicsBody( GameObject a_GameObject, const RigidBodyComponent& a_RigidBody );
		void DestroyPhysicsBody( PhysicsBodyID a_BodyID );
		bool HasPhysicsBody( PhysicsBodyID a_BodyID ) const;
		bool UpdatePhysicsBodyShape( GameObject a_GameObject, const RigidBodyComponent& a_RigidBody );

		Vector3 GetPhysicsBodyPosition( PhysicsBodyID a_BodyID ) const override;
		Quaternion GetPhysicsBodyRotation( PhysicsBodyID a_BodyID ) const override;
		Vector3 GetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID ) const override;
		Vector3 GetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID ) const override;
		bool IsPhysicsBodySleeping( PhysicsBodyID a_BodyID ) const override;

		void SetPhysicsBodyPosition( PhysicsBodyID a_BodyID, const Vector3& a_Position ) override;
		void SetPhysicsBodyRotation( PhysicsBodyID a_BodyID, const Quaternion& a_Rotation ) override;
		void SetPhysicsBodyPositionAndRotation( PhysicsBodyID a_BodyID, const Vector3& a_Position, const Quaternion& a_Rotation ) override;
		void SetPhysicsBodyFriction( PhysicsBodyID a_BodyID, float a_Friction ) override;
		void SetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID, const Vector3& a_LinearVelocity ) override;
		void SetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID, const Vector3& a_AngularVelocity ) override;
		void AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse ) override;
		void AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse, const Vector3& a_Position ) override;

	#if USE_DEBUG_RENDERER
		virtual void RenderDebug( const Matrix4& a_ViewProjection ) override;
	#endif

	protected:

		JPH::Ref<JPH::MutableCompoundShapeSettings> CreateShapeSettings( GameObject a_GameObject, const RigidBodyComponent& a_RigidBody, JPH::MassProperties& o_MassProps ) const;

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