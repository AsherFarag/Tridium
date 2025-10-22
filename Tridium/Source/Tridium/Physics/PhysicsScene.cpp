#include "tripch.h"
#include "PhysicsScene.h"
#include <Tridium/Physics/PhysicsComponents.h>

// Old
#include "PhysicsAPI.h"
#include "Jolt/JoltPhysicsScene.h"
#include <Tridium/Reflection/Reflection.h>
#include <Tridium/Containers/Set.h>
#include <Tridium/ECS/Components/Types.h>

namespace Tridium {

	BEGIN_REFLECT_ENUM( ESixDOFConstraintMotion )
		ENUM_VALUE( Locked )
		ENUM_VALUE( Limited )
		ENUM_VALUE( Free )
	END_REFLECT_ENUM( ESixDOFConstraintMotion );

	BEGIN_REFLECT( LinearMotionConstraint )
		PROPERTY( XMotion, EditAnywhere | Serialize )
		PROPERTY( XLimit,  EditAnywhere | Serialize )
		PROPERTY( YMotion, EditAnywhere | Serialize )
		PROPERTY( YLimit,  EditAnywhere | Serialize )
		PROPERTY( ZMotion, EditAnywhere | Serialize )
		PROPERTY( ZLimit,  EditAnywhere | Serialize )
	END_REFLECT( LinearMotionConstraint );

	BEGIN_REFLECT( AngularMotionConstraint )
		PROPERTY( Swing1Motion, EditAnywhere | Serialize )
		PROPERTY( Swing1Limit,  EditAnywhere | Serialize )
		PROPERTY( Swing2Motion, EditAnywhere | Serialize )
		PROPERTY( Swing2Limit,  EditAnywhere | Serialize )
		PROPERTY( TwistMotion,  EditAnywhere | Serialize )
		PROPERTY( TwistLimit,   EditAnywhere | Serialize )
	END_REFLECT( AngularMotionConstraint );

	UniquePtr<IPhysicsScene> IPhysicsScene::Create()
	{
		switch ( s_PhysicsAPI )
		{
		case EPhysicsAPI::Jolt:
			return MakeUnique<JoltPhysicsScene>();
		}

		ASSERT( false, "Unknown Physics API" );
		return nullptr;
	}

	void OldPhysicsSceneSystem::Init()
	{
		m_PhysicsScene = GetOwningScene()->GetPhysicsScene().get();
	}

	void OldPhysicsSceneSystem::Shutdown()
	{
		m_PhysicsScene = nullptr;
	}

	void OldPhysicsSceneSystem::OnSceneEvent( const SceneEventPayload& a_EventPayload )
	{
		if ( !GetOwningScene()->IsRunning() )
			return;

		switch ( a_EventPayload.EventType )
		{
		case ESceneEventType::OnComponentCreated:
		{
			const OnComponentCreatedEvent& event = std::get<OnComponentCreatedEvent>( a_EventPayload.EventData );
			OnComponentCreated( event );
			break;
		}

		case ESceneEventType::OnComponentDestroyed:
		{
			break;
		}
		default:
			break;
		}
	}

	void OldPhysicsSceneSystem::OnComponentCreated( const OnComponentCreatedEvent& a_Event )
	{
		static const Refl::MetaType RigidBodyComponentType = Refl::ResolveMetaType<OldRigidBodyComponent>();
		static const UnorderedSet<Refl::MetaIDType> ColliderComponentTypes =
		{
			Refl::ResolveMetaType<OldSphereColliderComponent>().ID(),
			Refl::ResolveMetaType<OldBoxColliderComponent>().ID(),
			Refl::ResolveMetaType<OldCapsuleColliderComponent>().ID(),
			Refl::ResolveMetaType<OldCylinderColliderComponent>().ID(),
			Refl::ResolveMetaType<OldMeshColliderComponent>().ID()
		};

		Refl::MetaType componentType = Refl::ResolveMetaType( a_Event.ComponentTypeID );
		OldGameObject gameObject = a_Event.GameObjectID;

		// Only handle RigidBody and Collider components
		if ( componentType != RigidBodyComponentType && !ColliderComponentTypes.contains( componentType.ID() ) )
		{
			return;
		}

		if ( OldRigidBodyComponent* rigidBody = gameObject.TryGetComponent<OldRigidBodyComponent>() )
		{
			if ( OldTransformComponent* transform = gameObject.TryGetComponent<OldTransformComponent>() )
			{
				m_PhysicsScene->UpdatePhysicsBody( gameObject, *rigidBody, *transform );
			}
		}
	}

	RayCastResult PhysicsSceneSystem::CastRay( const Vector3& a_Start, const Vector3& a_End, const RayCastParams& a_Params ) const
	{
		RayCastResult result = m_PhysicsScene->CastRay( a_Start, a_End, a_Params.Channel, a_Params.BodyFilter );
	
	#if TE_DRAW_DEBUG
		if ( a_Params.DrawDebug )
		{
			Debug::DrawLine( result.RayStart, result.RayStart + result.RayEnd, a_Params.Debug.LineColor, a_Params.Debug.DrawDurationType, a_Params.Debug.DrawDuration );
	
			if ( result.Hit )
			{
				AABBOld aabb = { result.Position - Vector3( 0.1f ), result.Position + Vector3( 0.1f ) };
				Debug::DrawAABBFilled( aabb, a_Params.Debug.HitColor, a_Params.Debug.DrawDurationType, a_Params.Debug.DrawDuration );
			}
		}
	#endif
	
		return result;
	}

	void PhysicsSceneSystem::Init()
	{
		// Create the physics scene and initialize it
		m_PhysicsScene = IPhysicsScene::Create();
		m_PhysicsScene->m_OwningScene = &OwningScene();
		m_PhysicsScene->Init();

		// Set up entity event handlers
		m_OnRigidBodyCreatedHandle = OwningScene().Registry().OnConstruct<RigidBodyComponent, &PhysicsSceneSystem::OnRigidBodyCreated>( this );
		m_OnRigidBodyDestroyedHandle = OwningScene().Registry().OnDestruct<RigidBodyComponent, &PhysicsSceneSystem::OnRigidBodyDestroyed>( this );
		m_OnColliderCreatedHandle = OwningScene().Registry().OnConstruct<SphereColliderComponent, &PhysicsSceneSystem::OnColliderCreated>( this );
		m_OnColliderDestroyedHandle = OwningScene().Registry().OnDestruct<SphereColliderComponent, &PhysicsSceneSystem::OnColliderDestroyed>( this );	
	}

	void PhysicsSceneSystem::Shutdown()
	{
		// NOTE: Since the physics scene can be shut down before any entities are destroyed,
		// we need to manually clear the physics scene reference from all rigid bodies.
		// This prevents dangling pointers in the RigidBodyComponents.
		auto rigidBodies = OwningScene().Registry().View<RigidBodyComponent>();
		rigidBodies.Each( [this]( EntityID a_Entity, RigidBodyComponent& a_RigidBody )
		{
			a_RigidBody.m_PhysicsScene = nullptr;
			a_RigidBody.m_BodyID = NullPhysicsBodyID;
		} );

		// Release entity event handlers
		m_OnRigidBodyCreatedHandle.Release();
		m_OnRigidBodyDestroyedHandle.Release(); 
		m_OnColliderCreatedHandle.Release(); 
		m_OnColliderDestroyedHandle.Release(); 

		m_PhysicsScene->Shutdown();
		m_PhysicsScene.reset();
	}

	void PhysicsSceneSystem::OnRigidBodyCreated( EntityComponentRegistry& a_Registry, EntityID a_Entity )
	{
		RigidBodyComponent& rigidBody = a_Registry.Get<RigidBodyComponent>( a_Entity );

		rigidBody.m_PhysicsScene = m_PhysicsScene.get();
		rigidBody.m_BodyID = m_PhysicsScene->CreatePhysicsBody( GameObject( OwningScene(), a_Entity ), rigidBody );
	}

	void PhysicsSceneSystem::OnRigidBodyDestroyed( EntityComponentRegistry& a_Registry, EntityID a_Entity )
	{
		RigidBodyComponent& rigidBody = a_Registry.Get<RigidBodyComponent>( a_Entity );
		if ( rigidBody.Valid() )
		{
			rigidBody.m_PhysicsScene->DestroyPhysicsBody( rigidBody.m_BodyID );
			rigidBody.m_BodyID = NullPhysicsBodyID;
			rigidBody.m_PhysicsScene = nullptr;
		}
	}

	void PhysicsSceneSystem::OnColliderCreated( EntityComponentRegistry& a_Registry, EntityID a_Entity )
	{
		if ( RigidBodyComponent* rigidBody = a_Registry.TryGet<RigidBodyComponent>( a_Entity ) )
		{
			if ( rigidBody->Valid() )
			{
				m_PhysicsScene->UpdatePhysicsBodyShape( GameObject( OwningScene(), a_Entity ), *rigidBody );
			}
		}
	}

	void PhysicsSceneSystem::OnColliderDestroyed( EntityComponentRegistry& a_Registry, EntityID a_Entity )
	{
		if ( RigidBodyComponent* rigidBody = a_Registry.TryGet<RigidBodyComponent>( a_Entity ) )
		{
			if ( rigidBody->Valid() )
			{
				m_PhysicsScene->UpdatePhysicsBodyShape( GameObject( OwningScene(), a_Entity ), *rigidBody );
			}
		}
	}

} // namespace Tridium
