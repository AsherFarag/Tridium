#include "tripch.h"
#include "JoltPhysicsScene.h"

namespace Tridium
{
	namespace Layer {
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
				case Layer::NON_MOVING:
					return inObject2 == Layer::MOVING; // Non moving only collides with moving
				case Layer::MOVING:
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
				mObjectToBroadPhase[Layer::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
				mObjectToBroadPhase[Layer::MOVING] = BroadPhaseLayers::MOVING;
			}

			virtual JPH::uint GetNumBroadPhaseLayers() const override
			{
				return BroadPhaseLayers::NUM_LAYERS;
			}

			virtual JPH::BroadPhaseLayer GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const override
			{
				JPH_ASSERT( inLayer < Layer::NUM_LAYERS );
				return mObjectToBroadPhase[inLayer];
			}
		private:
			JPH::BroadPhaseLayer mObjectToBroadPhase[Layer::NUM_LAYERS];
		};

		// Class that determines if an object layer can collide with a broadphase layer
		class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
		{
		public:
			virtual bool ShouldCollide( JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2 ) const override
			{
				switch ( inLayer1 )
				{
				case Layer::NON_MOVING:
					return inLayer2 == BroadPhaseLayers::MOVING;
				case Layer::MOVING:
					return true;
				default:
					JPH_ASSERT( false );
					return false;
				}
			}
		};
	}

	JoltPhysicsScene::JoltPhysicsScene()
	{
		// Register allocation hook.
		JPH::RegisterDefaultAllocator();

		// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
		JPH::Factory::sInstance = new JPH::Factory();

		JPH::RegisterTypes();

		m_JobSystem = MakeUnique<JPH::JobSystemThreadPool>( JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1 );
		m_TempAllocator = MakeUnique<JPH::TempAllocatorImpl>( 10 * 1024 * 1024 );
	}

	void JoltPhysicsScene::Init()
	{
		// Create the broad phase layer interface
		m_BroadPhaseLayerInterface = MakeUnique<Layer::BPLayerInterfaceImpl>();

		// Create the object vs broad phase layer filter
		m_ObjectVsBroadPhaseLayerFilter = MakeUnique<Layer::ObjectVsBroadPhaseLayerFilterImpl>();

		// Create the object layer pair filter
		m_ObjectLayerPairFilter = MakeUnique<Layer::ObjectLayerPairFilterImpl>();


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
		// Unregisters all types with the factory and cleans up the default material
		JPH::UnregisterTypes();

		// Delete the factory
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

	void JoltPhysicsScene::Tick( float a_TimeStep )
	{
		m_PhysicsSystem.Update( a_TimeStep, m_PhysicsSystemSettings.CollisionSteps, m_TempAllocator.get(), m_JobSystem.get());

		++m_CurrentStep;
	}

	PhysicsBodyID JoltPhysicsScene::AddPhysicsBody( void* a_CreationSettings )
	{
		JPH::BodyCreationSettings* bodySettings = (JPH::BodyCreationSettings*)a_CreationSettings;
		JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
		JPH::Body* body = bodyInterface.CreateBody( *bodySettings );
		if ( body )
		{
			bodyInterface.AddBody( body->GetID(), JPH::EActivation::Activate );
			return body->GetID().GetIndexAndSequenceNumber();
		}

		return JPH::BodyID::cInvalidBodyID;
	}
}
