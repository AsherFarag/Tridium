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

	UniquePtr<PhysicsScene> PhysicsScene::Create()
	{
		switch ( s_PhysicsAPI )
		{
		case EPhysicsAPI::Jolt:
			return MakeUnique<JoltPhysicsScene>();
		}

		ASSERT_LOG( false, "Unknown Physics API" );
		return nullptr;
	}

	void PhysicsSceneSystem::Init()
	{
		m_PhysicsScene = GetOwningScene()->GetPhysicsScene().get();
	}

	void PhysicsSceneSystem::Shutdown()
	{
		m_PhysicsScene = nullptr;
	}

	void PhysicsSceneSystem::OnSceneEvent( const SceneEventPayload& a_EventPayload )
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

	void PhysicsSceneSystem::OnComponentCreated( const OnComponentCreatedEvent& a_Event )
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
		GameObject gameObject = a_Event.GameObjectID;

		// Only handle RigidBody and Collider components
		if ( componentType != RigidBodyComponentType && !ColliderComponentTypes.contains( componentType.ID() ) )
		{
			return;
		}

		if ( RigidBodyComponent* rigidBody = gameObject.TryGetComponent<RigidBodyComponent>() )
		{
			if ( TransformComponent* transform = gameObject.TryGetComponent<TransformComponent>() )
			{
				m_PhysicsScene->UpdatePhysicsBody( gameObject, *rigidBody, *transform );
			}
		}
	}
}
