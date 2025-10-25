#include "tripch.h"
#include <Tridium/Physics/PhysicsComponents.h>
#include <Tridium/Physics/PhysicsLayer.h>
#include <Tridium/Physics/Jolt/JoltPhysicsScene.h>
#include <Tridium/Physics/Jolt/JoltUtil.h>
#include <Tridium/Physics/Jolt/JoltColliders.h>
#include <Tridium/Physics/Jolt/JoltPhysicsFilter.h>
#include <Tridium/Physics/Jolt/JoltDebugRenderer.h>

#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/CompoundShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

namespace Tridium {
	namespace Layers {

		TODO( "Set up a proper Physics Layer Manager Initialiser. Probably in the Project Settings." )
		static PhysicsLayerManager s_PhysicsLayerManager;

		static PhysicsLayerMask s_StaticPhysicsLayerMask;
		static PhysicsLayerMask s_DynamicPhysicsLayerMask;
		static PhysicsLayerMask s_PlayerPhysicsLayerMask;

		static bool s_PhysicsLayerMasksInitialised = (
			s_StaticPhysicsLayerMask.SetCollisionResponse( EPhysicsLayer::Static, ECollisionResponse::Ignore ),
			s_StaticPhysicsLayerMask.SetCollisionResponse( EPhysicsLayer::Dynamic, ECollisionResponse::Block ),
			s_StaticPhysicsLayerMask.SetCollisionResponse( EPhysicsLayer::Player, ECollisionResponse::Block ),
			s_DynamicPhysicsLayerMask.SetCollisionResponse( EPhysicsLayer::Static, ECollisionResponse::Block ),
			s_DynamicPhysicsLayerMask.SetCollisionResponse( EPhysicsLayer::Dynamic, ECollisionResponse::Block ),
			s_DynamicPhysicsLayerMask.SetCollisionResponse( EPhysicsLayer::Player, ECollisionResponse::Block ),
			s_PlayerPhysicsLayerMask.SetCollisionResponse( EPhysicsLayer::Static, ECollisionResponse::Block ),
			s_PlayerPhysicsLayerMask.SetCollisionResponse( EPhysicsLayer::Dynamic, ECollisionResponse::Block ),
			s_PlayerPhysicsLayerMask.SetCollisionResponse( EPhysicsLayer::Player, ECollisionResponse::Block ),
			true
			);

		static bool s_PhysicsLayerManagerInitialised = (
			s_PhysicsLayerManager.SetLayerMask( EPhysicsLayer::Static, s_StaticPhysicsLayerMask ),
			s_PhysicsLayerManager.SetLayerMask( EPhysicsLayer::Dynamic, s_DynamicPhysicsLayerMask ),
			s_PhysicsLayerManager.SetLayerMask( EPhysicsLayer::Player, s_PlayerPhysicsLayerMask ),
			true
			);

		// Class that determines if two object layers can collide
		class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
		{
		public:
			virtual bool ShouldCollide( JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2 ) const override
			{
				ECollisionResponse response = s_PhysicsLayerManager.GetCollisionResponse( Cast<EPhysicsLayer>( inObject1 ), Cast<EPhysicsLayer>( inObject2 ) );
				return response == ECollisionResponse::Block;
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
				mObjectToBroadPhase[Cast<JPH::ObjectLayer>( EPhysicsLayer::Static  )] = BroadPhaseLayers::NON_MOVING;
				mObjectToBroadPhase[Cast<JPH::ObjectLayer>( EPhysicsLayer::Dynamic )] = BroadPhaseLayers::MOVING;
				mObjectToBroadPhase[Cast<JPH::ObjectLayer>( EPhysicsLayer::Player  )] = BroadPhaseLayers::MOVING;
			}

			virtual JPH::uint GetNumBroadPhaseLayers() const override
			{
				return BroadPhaseLayers::NUM_LAYERS;
			}

			virtual JPH::BroadPhaseLayer GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const override
			{
				JPH_ASSERT( inLayer < Cast<JPH::ObjectLayer>( EPhysicsLayer::NUM_LAYERS ) );
				return mObjectToBroadPhase[inLayer];
			}
		private:
			JPH::BroadPhaseLayer mObjectToBroadPhase[(size_t)EPhysicsLayer::NUM_LAYERS];
		};

		// Class that determines if an object layer can collide with a broadphase layer
		class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
		{
		public:
			virtual bool ShouldCollide( JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2 ) const override
			{
				switch ( Cast<EPhysicsLayer>( inLayer1 ) )
				{
				case EPhysicsLayer::Static:
					return inLayer2 == BroadPhaseLayers::MOVING;
				case EPhysicsLayer::Dynamic:
					return true;
				case EPhysicsLayer::Player:
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

		m_PhysicsSystem.OptimizeBroadPhase();

		m_Initialised = true;
	}

	void JoltPhysicsScene::Shutdown()
	{

	}

	void JoltPhysicsScene::Tick( float a_TimeStep )
	{
		m_PhysicsSystem.Update( a_TimeStep, m_PhysicsSystemSettings.CollisionSteps, m_TempAllocator.get(), m_JobSystem.get());

		++m_CurrentStep;
	}

	RayCastResult JoltPhysicsScene::CastRay( const Vector3& a_Start, const Vector3& a_End, ERayCastChannel a_Channel, const PhysicsBodyFilter& a_BodyFilter )
	{
		JPH::RRayCast ray( Util::ToJoltVec3( a_Start ), Util::ToJoltVec3( a_End ) );
		JPH::RayCastResult rayResult;

		RayCastResult hit = { false, Vector3( 0.0f ), Vector3( 0.0f ), 0.0f, a_Start, a_End, {} };

		if ( m_PhysicsSystem.GetNarrowPhaseQuery().CastRay( ray, rayResult, {}, JoltObjectLayerFilter(a_Channel), JoltBodyFilter(a_BodyFilter)) )
		{
			hit.Hit = true;
			hit.Position = Util::ToTridiumVec3( ray.GetPointOnRay( rayResult.mFraction ) );
			hit.Normal = Util::ToTridiumVec3( ray.mDirection ).Normalized();
			hit.Distance = Math::Distance( a_Start, hit.Position );
			hit.HitBodyID = rayResult.mBodyID.GetIndexAndSequenceNumber();
			TODO( "THis" );
			NOT_IMPLEMENTED;
			//hit.HitGameObject = GetGameObjectFromPhysicsBody( hit.HitBodyID );
		}

		return hit;
	}

	PhysicsBodyID JoltPhysicsScene::CreatePhysicsBody( GameObject a_GameObject, const RigidBodyComponent& a_RigidBody )
	{
		ASSERT( m_Initialised );

		if ( !a_GameObject.HasAny<SphereColliderComponent, BoxColliderComponent, CapsuleColliderComponent, CylinderColliderComponent, MeshColliderComponent>() )
		{
			return NullPhysicsBodyID;
		}

		const JPH::Vec3 position = {};//Util::ToJoltVec3( a_TransformComponent.GetWorldPosition() );
		const JPH::Quat rotation = {};//Util::ToJoltQuat( a_TransformComponent.GetOrientation() );
		const JPH::Vec3 scale    = {};//Util::ToJoltVec3( a_TransformComponent.GetWorldScale() );

		JPH::MassProperties massProperties;
		JPH::Ref<JPH::MutableCompoundShapeSettings> compoundSettings = CreateShapeSettings( a_GameObject, a_RigidBody, massProperties );

		if ( !compoundSettings )
		{
			return NullPhysicsBodyID;
		}

		massProperties.Scale( Util::ToJoltVec3( Vector3( a_RigidBody.MassScale() ) ) );

		// Create the body creation settings
		const JPH::EMotionType motionType = Util::ToJoltMotionType( a_RigidBody.MotionType() );
		const JPH::ObjectLayer layer = Cast<JPH::ObjectLayer>( a_RigidBody.PhysicsLayer() );
		JPH::BodyCreationSettings bodySettings( compoundSettings, position, rotation, motionType, layer );
		bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::MassAndInertiaProvided;
		bodySettings.mMassPropertiesOverride = massProperties;
		bodySettings.mRestitution = a_RigidBody.Restitution();

		JPH::Body* body = m_BodyInterface.CreateBody( bodySettings );

		if ( !body )
		{
			// Failed to create body
			return NullPhysicsBodyID;
		}

		// Successfully created body

		// Set six degrees of freedom constraint
		{
			bool needsConstraint = false;
			const LinearMotionConstraint& linearMotionConstraint = a_RigidBody.LinearMotionConstraint();
			needsConstraint |= linearMotionConstraint.XMotion != ESixDOFConstraintMotion::Free;
			needsConstraint |= linearMotionConstraint.YMotion != ESixDOFConstraintMotion::Free;
			needsConstraint |= linearMotionConstraint.ZMotion != ESixDOFConstraintMotion::Free;

			const AngularMotionConstraint& angularMotionConstraint = a_RigidBody.AngularMotionConstraint();
			needsConstraint |= angularMotionConstraint.Swing1Motion != ESixDOFConstraintMotion::Free;
			needsConstraint |= angularMotionConstraint.Swing2Motion != ESixDOFConstraintMotion::Free;
			needsConstraint |= angularMotionConstraint.TwistMotion != ESixDOFConstraintMotion::Free;
			if ( needsConstraint )
			{
				JPH::SixDOFConstraintSettings* constraintSettings = new JPH::SixDOFConstraintSettings();

				if ( angularMotionConstraint.Swing1Motion == ESixDOFConstraintMotion::Locked )
					constraintSettings->SetLimitedAxis( JPH::SixDOFConstraintSettings::RotationX, 0.0f, 0.0f );

				if ( angularMotionConstraint.Swing2Motion == ESixDOFConstraintMotion::Locked )
					constraintSettings->SetLimitedAxis( JPH::SixDOFConstraintSettings::RotationY, 0.0f, 0.0f );

				if ( angularMotionConstraint.TwistMotion == ESixDOFConstraintMotion::Locked )
					constraintSettings->SetLimitedAxis( JPH::SixDOFConstraintSettings::RotationZ, 0.0f, 0.0f );


				// Add the constraint to the body
				JPH::Constraint* constraint = m_BodyInterface.CreateConstraint( constraintSettings, body->GetID(), JPH::BodyID() );
				m_PhysicsSystem.AddConstraint( constraint );
			}
		}

		if ( a_RigidBody.MotionType() != EMotionType::Static )
		{
			body->GetMotionProperties()->SetGravityFactor( a_RigidBody.GravityScale() );
		}

		m_BodyInterface.AddBody( body->GetID(), JPH::EActivation::Activate );
		m_BodyInterface.SetUserData( body->GetID(), Cast<uint64_t>( a_GameObject.ID() ) );
		return body->GetID().GetIndexAndSequenceNumber();
	}

	void JoltPhysicsScene::DestroyPhysicsBody( PhysicsBodyID a_BodyID )
	{
		ASSERT( m_Initialised );

		if ( a_BodyID == JPH::BodyID::cInvalidBodyID )
			return;

		m_BodyInterface.RemoveBody( JPH::BodyID( a_BodyID ) );
		m_BodyInterface.DestroyBody( JPH::BodyID( a_BodyID ) );
	}

	bool JoltPhysicsScene::HasPhysicsBody( PhysicsBodyID a_BodyID ) const
	{
		ASSERT( m_Initialised );

		if ( a_BodyID == JPH::BodyID::cInvalidBodyID )
			return false;

		// Jolt doesn't have a way to check if a body exists so just return true if it is valid.
		return true;
	}

	bool JoltPhysicsScene::UpdatePhysicsBodyShape( GameObject a_GameObject, const RigidBodyComponent& a_RigidBody )
	{
		ASSERT( m_Initialised );
		
		if ( !a_RigidBody.Valid() )
		{
			return false;
		}

		JPH::MassProperties massProperties;
		JPH::Ref<JPH::MutableCompoundShapeSettings> compoundSettings = CreateShapeSettings( a_GameObject, a_RigidBody, massProperties );

		if ( !compoundSettings )
		{
			return false;
		}

		massProperties.Scale( Util::ToJoltVec3( Vector3( a_RigidBody.MassScale() ) ) );

		JPH::Ref<JPH::Shape> shape = compoundSettings->Create().Get();

		m_BodyInterface.SetShape( JPH::BodyID( a_RigidBody.BodyID() ), shape.GetPtr(), false, JPH::EActivation::Activate );

		return true;
	}

	Vector3 JoltPhysicsScene::GetPhysicsBodyPosition( PhysicsBodyID a_BodyID ) const
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		return Util::ToTridiumVec3( m_BodyInterface.GetPosition( JPH::BodyID( a_BodyID ) ) );
	}

	Quaternion JoltPhysicsScene::GetPhysicsBodyRotation( PhysicsBodyID a_BodyID ) const
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		return Util::ToTridiumQuat( m_BodyInterface.GetRotation( JPH::BodyID( a_BodyID ) ) );
	}

	Vector3 JoltPhysicsScene::GetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID ) const
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		return Util::ToTridiumVec3( m_BodyInterface.GetLinearVelocity( JPH::BodyID( a_BodyID ) ) );
	}

	Vector3 JoltPhysicsScene::GetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID ) const
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		return Util::ToTridiumVec3( m_BodyInterface.GetAngularVelocity( JPH::BodyID( a_BodyID ) ) );
	}

	bool JoltPhysicsScene::IsPhysicsBodySleeping( PhysicsBodyID a_BodyID ) const
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		return m_BodyInterface.IsActive( JPH::BodyID( a_BodyID ) );
	}

	void JoltPhysicsScene::SetPhysicsBodyPosition( PhysicsBodyID a_BodyID, const Vector3& a_Position )
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		m_BodyInterface.SetPosition( JPH::BodyID( a_BodyID ), Util::ToJoltVec3( a_Position ), JPH::EActivation::Activate );
	}

	void JoltPhysicsScene::SetPhysicsBodyRotation( PhysicsBodyID a_BodyID, const Quaternion& a_Rotation )
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		m_BodyInterface.SetRotation( JPH::BodyID( a_BodyID ), Util::ToJoltQuat( a_Rotation ), JPH::EActivation::Activate );
	}

	void JoltPhysicsScene::SetPhysicsBodyFriction( PhysicsBodyID a_BodyID, float a_Friction )
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		m_BodyInterface.SetFriction( JPH::BodyID( a_BodyID ), a_Friction );
	}

	void JoltPhysicsScene::SetPhysicsBodyLinearVelocity( PhysicsBodyID a_BodyID, const Vector3& a_LinearVelocity )
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		m_BodyInterface.SetLinearVelocity( JPH::BodyID( a_BodyID ), Util::ToJoltVec3( a_LinearVelocity ) );
	}

	void JoltPhysicsScene::SetPhysicsBodyAngularVelocity( PhysicsBodyID a_BodyID, const Vector3& a_AngularVelocity )
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		m_BodyInterface.SetAngularVelocity( JPH::BodyID( a_BodyID ), Util::ToJoltVec3( a_AngularVelocity ) );
	}

	void JoltPhysicsScene::AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse )
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		m_BodyInterface.AddImpulse( JPH::BodyID( a_BodyID ), Util::ToJoltVec3( a_Impulse ) );
	}

	void JoltPhysicsScene::AddImpulseToPhysicsBody( PhysicsBodyID a_BodyID, const Vector3& a_Impulse, const Vector3& a_Position )
	{
		ASSERT( a_BodyID != JPH::BodyID::cInvalidBodyID );
		m_BodyInterface.AddImpulse( JPH::BodyID( a_BodyID ), Util::ToJoltVec3( a_Impulse ), Util::ToJoltVec3( a_Position ) );
	}

#if USE_DEBUG_RENDERER

	void JoltPhysicsScene::RenderDebug( const Matrix4& a_ViewProjection )
	{
		if ( JPH::DebugRenderer::sInstance )
		{
			JPH::BodyManager::DrawSettings drawSettings;
			drawSettings.mDrawShape = true;
			drawSettings.mDrawShapeWireframe = true;
			drawSettings.mDrawCenterOfMassTransform = true;
			m_PhysicsSystem.DrawBodies( drawSettings, JPH::DebugRenderer::sInstance );
			//m_PhysicsSystem.DrawConstraints( JPH::DebugRenderer::sInstance );

			Cast<JoltDebugRenderer*>( JPH::DebugRenderer::sInstance )->Render( a_ViewProjection );
		}
	}

	JPH::Ref<JPH::MutableCompoundShapeSettings> JoltPhysicsScene::CreateShapeSettings( GameObject a_GameObject, const RigidBodyComponent& a_RigidBody, JPH::MassProperties& o_MassProps ) const
	{
		JPH::Ref<JPH::MutableCompoundShapeSettings> result = new JPH::MutableCompoundShapeSettings();

		uint32_t numShapes = 0;

		// Add all ColliderComponents to the compound shape
		{
			// Sphere Collider
			if ( auto* sc = a_GameObject.TryGet<SphereColliderComponent>() )
			{
				const JPH::Ref<JPH::SphereShape> sphereShape = new JPH::SphereShape( sc->Radius() );
				const JPH::Vec3 center = Util::ToJoltVec3( sc->Center() );
				const JPH::Quat rotation = Util::ToJoltQuat( sc->Rotation().Quat );
				result->AddShape( center, rotation, sphereShape );
				o_MassProps.mMass += sphereShape->GetMassProperties().mMass;
				o_MassProps.mInertia += sphereShape->GetMassProperties().mInertia;
				++numShapes;
			}

			// Box Collider
			if ( auto* bc = a_GameObject.TryGet<BoxColliderComponent>() )
			{
				const JPH::Vec3 halfExtents = Util::ToJoltVec3( bc->HalfExtents() );
				const JPH::Ref<JPH::BoxShape> boxShape = new JPH::BoxShape( halfExtents );
				const JPH::Vec3 center = Util::ToJoltVec3( bc->Center() );
				const JPH::Quat rotation = Util::ToJoltQuat( bc->Rotation().Quat );
				result->AddShape( center, rotation, boxShape );
				o_MassProps.mMass += boxShape->GetMassProperties().mMass;
				o_MassProps.mInertia += boxShape->GetMassProperties().mInertia;
				++numShapes;
			}

			// Capsule Collider
			if ( auto* cc = a_GameObject.TryGet<CapsuleColliderComponent>() )
			{
				const JPH::Ref<JPH::CapsuleShape> capsuleShape = new JPH::CapsuleShape( cc->HalfHeight(), cc->Radius() );
				const JPH::Vec3 center = Util::ToJoltVec3( cc->Center() );
				const JPH::Quat rotation = Util::ToJoltQuat( cc->Rotation().Quat );
				result->AddShape( center, rotation, capsuleShape );
				o_MassProps.mMass += capsuleShape->GetMassProperties().mMass;
				o_MassProps.mInertia += capsuleShape->GetMassProperties().mInertia;
				++numShapes;
			}

			// Cylinder Collider
			if ( auto* cc = a_GameObject.TryGet<CylinderColliderComponent>() )
			{
				const JPH::Ref<JPH::CylinderShape> cylinderShape = new JPH::CylinderShape( cc->HalfHeight(), cc->Radius() );
				const JPH::Vec3 center = Util::ToJoltVec3( cc->Center() );
				const JPH::Quat rotation = Util::ToJoltQuat( cc->Rotation().Quat );
				result->AddShape( center, rotation, cylinderShape );
				o_MassProps.mMass += cylinderShape->GetMassProperties().mMass;
				o_MassProps.mInertia += cylinderShape->GetMassProperties().mInertia;
				++numShapes;
			}

			// Mesh Collider
			//if ( auto* mc = a_GameObject.TryGet<MeshColliderComponent>() )
			if ( false )
			{
				TODO( "Implement MeshColliderComponent support in JoltPhysicsScene::CreatePhysicsBody" );
				NOT_IMPLEMENTED;
				//if ( auto staticMesh = AssetManager::GetAsset<OldStaticMesh>( mc->GetMesh() ) )
				//{
				//	if ( auto meshSource = AssetManager::GetAsset<MeshSource>( staticMesh->GetMeshSource() ) )
				//	{
				//		for ( uint32_t subMeshIndex : staticMesh->GetSubMeshes() )
				//		{
				//			const OldSubMesh& subMesh = meshSource->GetSubMesh( subMeshIndex );
				//			SharedPtr<JoltMeshCollider> meshCollider = SharedPtrCast<JoltMeshCollider>( subMesh.Collider );
				//			if ( !meshCollider || !meshCollider->IsValid() )
				//				continue;

				//			Vector3 meshPosition;
				//			Quaternion meshRotation;
				//			Vector3 meshScale;
				//			Math::DecomposeTransform( subMesh.Transform, meshPosition, meshRotation, meshScale );
				//			meshCollider->GetMeshShape()->ScaleShape( scale );
				//			result->AddShape( Util::ToJoltVec3( meshPosition ), Util::ToJoltQuat( meshRotation ), meshCollider->GetMeshShape() );
				//			o_MassProps.mMass += meshCollider->GetMassProperties().mMass;
				//			o_MassProps.mInertia += meshCollider->GetMassProperties().mInertia;
				//			++numShapes;
				//		}
				//	}
				//}
			}
		}


		if ( numShapes == 0 )
		{
			return nullptr;
		}

		return result;
	}

#endif

} // namespace Tridium
