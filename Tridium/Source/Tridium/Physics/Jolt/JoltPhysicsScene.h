#pragma once
#include <Tridium/Physics/PhysicsScene.h>
#include <Tridium/Physics/PhysicsLayer.h>

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

		virtual PhysicsBodyID AddPhysicsBody( void* a_CreationSettings ) override;

	protected:
		PhysicsLayerManager m_LayerManager;
		JPH::PhysicsSystem m_PhysicsSystem;

		UniquePtr<JPH::JobSystemThreadPool> m_JobSystem;
		UniquePtr<JPH::TempAllocatorImpl> m_TempAllocator;
		UniquePtr<JPH::BroadPhaseLayerInterface> m_BroadPhaseLayerInterface;
		UniquePtr<JPH::ObjectVsBroadPhaseLayerFilter> m_ObjectVsBroadPhaseLayerFilter;
		UniquePtr<JPH::ObjectLayerPairFilter> m_ObjectLayerPairFilter;

		struct
		{
			const uint32_t MaxBodies = 65536;
			const uint32_t NumBodyMutexes = 0;
			const uint32_t MaxBodyPairs = 65536;
			const uint32_t MaxContactConstraints = 10240;
			const uint32_t CollisionSteps = 1;
		} m_PhysicsSystemSettings;

		uint64_t m_CurrentStep = 0;

		// TEMP!
		friend class Scene;
	};

}