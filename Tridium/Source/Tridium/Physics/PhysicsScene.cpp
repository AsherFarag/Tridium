#include "tripch.h"
#include "PhysicsScene.h"
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
		static const Refl::MetaType RigidBodyComponentType = Refl::ResolveMetaType<RigidBodyComponent>();
		static const UnorderedSet<Refl::MetaIDType> ColliderComponentTypes =
		{
			Refl::ResolveMetaType<SphereColliderComponent>().ID(),
			Refl::ResolveMetaType<BoxColliderComponent>().ID(),
			Refl::ResolveMetaType<CapsuleColliderComponent>().ID(),
			Refl::ResolveMetaType<CylinderColliderComponent>().ID(),
			Refl::ResolveMetaType<MeshColliderComponent>().ID()
		};

		Refl::MetaType componentType = Refl::ResolveMetaType( a_Event.ComponentTypeID );
		OldGameObject gameObject = a_Event.GameObjectID;

		// Only handle RigidBody and Collider components
		if ( componentType != RigidBodyComponentType && !ColliderComponentTypes.contains( componentType.ID() ) )
		{
			return;
		}

		if ( RigidBodyComponent* rigidBody = gameObject.TryGetComponent<RigidBodyComponent>() )
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
		m_PhysicsScene = IPhysicsScene::Create();
		m_PhysicsScene->m_OwningScene = &OwningScene();

		TODO( "Iterate over physics components and set them up" );
	}

	void PhysicsSceneSystem::Shutdown()
	{
		m_PhysicsScene->Shutdown();
		m_PhysicsScene.reset();
	}

	void PhysicsSceneSystem::OnRigidBodyComponentCreated( EntityComponentRegistry& a_Registry, EntityID a_Entity )
	{
	}

}
