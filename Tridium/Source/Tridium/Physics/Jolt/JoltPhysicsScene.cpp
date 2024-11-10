#include "tripch.h"
#include "JoltPhysicsScene.h"
#include "JoltUtil.h"

#include <Tridium/ECS/Components/Types.h>

#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/CompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

namespace Tridium
{
	namespace Layers {
		static constexpr JPH::ObjectLayer NON_MOVING = 0;
		static constexpr JPH::ObjectLayer MOVING = 1;
		static constexpr JPH::ObjectLayer NUM_LAYERS = 2;

		// Class that determines if two object layers can collide
		class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
		{
		public:
			virtual bool ShouldCollide( JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2 ) const override
			{
				switch ( inObject1 )
				{
				case Layers::NON_MOVING:
					return inObject2 == Layers::MOVING; // Non moving only collides with moving
				case Layers::MOVING:
					return true; // Moving collides with everything
				default:
					JPH_ASSERT( false );
					return false;
				}
			}
		};

		// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
		// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
		// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
		// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
		// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
		namespace BroadPhaseLayers
		{
			static constexpr JPH::BroadPhaseLayer NON_MOVING( 0 );
			static constexpr JPH::BroadPhaseLayer MOVING( 1 );
			static constexpr JPH::uint NUM_LAYERS( 2 );
		};

		// BroadPhaseLayerInterface implementation
		// This defines a mapping between object and broadphase layers.
		class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
		{
		public:
			BPLayerInterfaceImpl()
			{
				// Create a mapping table from object to broad phase layer
				mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
				mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
			}

			virtual JPH::uint GetNumBroadPhaseLayers() const override
			{
				return BroadPhaseLayers::NUM_LAYERS;
			}

			virtual JPH::BroadPhaseLayer GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const override
			{
				JPH_ASSERT( inLayer < Layers::NUM_LAYERS );
				return mObjectToBroadPhase[inLayer];
			}
		private:
			JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
		};

		// Class that determines if an object layer can collide with a broadphase layer
		class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
		{
		public:
			virtual bool ShouldCollide( JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2 ) const override
			{
				switch ( inLayer1 )
				{
				case Layers::NON_MOVING:
					return inLayer2 == BroadPhaseLayers::MOVING;
				case Layers::MOVING:
					return true;
				default:
					JPH_ASSERT( false );
					return false;
				}
			}
		};
	}

	JoltPhysicsScene::JoltPhysicsScene()
		: m_BodyInterface( m_PhysicsSystem.GetBodyInterface() )
	{
		m_JobSystem = MakeUnique<JPH::JobSystemThreadPool>( JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1 );
		m_TempAllocator = MakeUnique<JPH::TempAllocatorImpl>( 10 * 1024 * 1024 );
	}

	void JoltPhysicsScene::Init()
	{
		// Create the broad phase layer interface
		m_BroadPhaseLayerInterface = MakeUnique<Layers::BPLayerInterfaceImpl>();

		// Create the object vs broad phase layer filter
		m_ObjectVsBroadPhaseLayerFilter = MakeUnique<Layers::ObjectVsBroadPhaseLayerFilterImpl>();

		// Create the object layer pair filter
		m_ObjectLayerPairFilter = MakeUnique<Layers::ObjectLayerPairFilterImpl>();


		// Init the physics system with the settings we defined
		m_PhysicsSystem.Init(
			m_PhysicsSystemSettings.MaxBodies,
			m_PhysicsSystemSettings.NumBodyMutexes,
			m_PhysicsSystemSettings.MaxBodyPairs,
			m_PhysicsSystemSettings.MaxContactConstraints,
			*m_BroadPhaseLayerInterface,
			*m_ObjectVsBroadPhaseLayerFilter,
			*m_ObjectLayerPairFilter );
	}

	void JoltPhysicsScene::Shutdown()
	{

	}

	void JoltPhysicsScene::Tick( float a_TimeStep )
	{
		m_PhysicsSystem.Update( a_TimeStep, m_PhysicsSystemSettings.CollisionSteps, m_TempAllocator.get(), m_JobSystem.get());

		++m_CurrentStep;
	}

	void JoltPhysicsScene::RemovePhysicsBody( RigidBodyComponent& a_RigidBody )
	{
		m_BodyInterface.RemoveBody( JPH::BodyID( a_RigidBody.BodyID ) );
		a_RigidBody.BodyID = JPH::BodyID::cInvalidBodyID;
	}

	bool JoltPhysicsScene::AddPhysicsBody( const GameObject& a_GameObject, RigidBodyComponent& a_RigidBody, TransformComponent& a_TransformComponent )
	{
		a_RigidBody.BodyID = JPH::BodyID::cInvalidBodyID;

		const Vector3 scale = a_TransformComponent.GetWorldScale();

		JPH::Ref<JPH::MutableCompoundShapeSettings> compoundSettings = new JPH::MutableCompoundShapeSettings();
		uint32_t numShapes = 0;

		// Add all ColliderComponents to the compound shape
		{
			// Sphere Collider
			if ( auto* sc = m_Scene->TryGetComponentFromGameObject<SphereColliderComponent>( a_GameObject ) )
			{
				const JPH::Ref<JPH::SphereShape> sphereShape = new JPH::SphereShape( sc->GetRadius() );
				const JPH::Vec3 center = Util::ToJoltVec3( sc->GetCenter() );
				compoundSettings->AddShape( center, JPH::Quat::sIdentity(), sphereShape );

				++numShapes;
			}
			// Box Collider
			if ( auto* bc = m_Scene->TryGetComponentFromGameObject<BoxColliderComponent>( a_GameObject ) )
			{
				const JPH::Vec3 halfExtents = JPH::Vec3( bc->GetHalfExtents().x * scale.x, bc->GetHalfExtents().y * scale.y, bc->GetHalfExtents().z * scale.z );
				const JPH::Ref<JPH::BoxShape> boxShape = new JPH::BoxShape( halfExtents );
				const JPH::Vec3 center = Util::ToJoltVec3( bc->GetCenter() );
				compoundSettings->AddShape( center, JPH::Quat::sIdentity(), boxShape );

				++numShapes;
			}
			// Capsule Collider
			if ( auto* cc = m_Scene->TryGetComponentFromGameObject<CapsuleColliderComponent>( a_GameObject ) )
			{
				const JPH::Ref<JPH::CapsuleShape> capsuleShape = new JPH::CapsuleShape( cc->GetHalfHeight(), cc->GetRadius() );
				const JPH::Vec3 center = Util::ToJoltVec3( cc->GetCenter() );
				compoundSettings->AddShape( center, JPH::Quat::sIdentity(), capsuleShape );

				++numShapes;
			}

			// Cylinder Collider
			if ( auto* cc = m_Scene->TryGetComponentFromGameObject<CylinderColliderComponent>( a_GameObject ) )
			{
				const JPH::Ref<JPH::CylinderShape> cylinderShape = new JPH::CylinderShape( cc->GetHalfHeight(), cc->GetRadius() );
				const JPH::Vec3 center = Util::ToJoltVec3( cc->GetCenter() );
				compoundSettings->AddShape( center, JPH::Quat::sIdentity(), cylinderShape );

				++numShapes;
			}
		}

		// Failed to add body if there are no shapes
		if ( numShapes == 0 )
		{
			return false;
		}

		// Create the body creation settings
		const JPH::Vec3 position = Util::ToJoltVec3( a_TransformComponent.GetWorldPosition() );
		const JPH::Quat rotation = Util::ToJoltQuat( a_TransformComponent.GetOrientation() );
		const JPH::EMotionType motionType = Util::ToJoltMotionType( a_RigidBody.GetMotionType() );
		const JPH::ObjectLayer layer = motionType == JPH::EMotionType::Static ? Layers::NON_MOVING : Layers::MOVING;
		JPH::BodyCreationSettings bodySettings( compoundSettings, position, rotation, motionType, layer );
		bodySettings.mRestitution = 0.5f; // TEMP!

		if ( JPH::Body* body = m_BodyInterface.CreateBody( bodySettings ) )
		{
			// Successfully created body

			if ( a_RigidBody.GetMotionType() != EMotionType::Static )
			{
				body->GetMotionProperties()->SetGravityFactor( a_RigidBody.GetGravityScale() );
			}

			m_BodyInterface.AddBody( body->GetID(), JPH::EActivation::Activate );
			a_RigidBody.BodyID = body->GetID().GetIndexAndSequenceNumber();
			return true;
		}

		// Failed to create body
		return false;
	}

	bool JoltPhysicsScene::UpdatePhysicsBody( const GameObject& a_GameObject, RigidBodyComponent& a_RigidBody, TransformComponent& a_TransformComponent )
	{
		RemovePhysicsBody( a_RigidBody );
		return AddPhysicsBody( a_GameObject, a_RigidBody, a_TransformComponent );
	}

	void JoltPhysicsScene::UpdatePhysicsBodyTransform( const RigidBodyComponent& a_RigidBody, const TransformComponent& a_TransformComponent )
	{
		m_BodyInterface.SetPositionAndRotation( 
			JPH::BodyID( a_RigidBody.BodyID ),
			Util::ToJoltVec3( a_TransformComponent.GetWorldPosition() ),
			Util::ToJoltQuat( a_TransformComponent.GetOrientation() ),
			JPH::EActivation::Activate );
	}
}
