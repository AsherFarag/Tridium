#pragma once
#include <Tridium/ECS/ECS.h>
#include <Tridium/Reflection/ReflectionFwd.h>
#include <variant>

namespace Tridium {

	// Forward declarations
	class Component;
	// -------------------

	enum class ESceneEventType : uint8_t
	{
		None = 0,
		OnBeginPlay,
		OnUpdate,
		OnEndPlay,

		// ================================

		OnGameObjectCreated,
		OnGameObjectCopied,
		OnGameObjectDestroyed,
		OnComponentCreated,
		OnComponentDestroyed,

		// ================================

	};

	struct OnGameObjectCreatedEvent
	{
		EntityID GameObjectID = NullEntity;
	};

	struct OnGameObjectCopiedEvent
	{
		EntityID SourceID = NullEntity;
		EntityID DestinationID = NullEntity;
	};

	struct OnGameObjectDestroyedEvent
	{
		EntityID GameObjectID = NullEntity;
	};

	struct OnComponentCreatedEvent
	{
		EntityID GameObjectID = NullEntity;
		Refl::MetaIDType ComponentTypeID = Refl::INVALID_META_ID;
		Component* Component = nullptr;
	};

	struct OnComponentDestroyedEvent
	{
		Refl::MetaIDType ComponentTypeID = Refl::INVALID_META_ID;
		Component* Component = nullptr;
	};

	struct SceneEventPayload
	{
		ESceneEventType EventType = ESceneEventType::None;
		std::variant<
			OnGameObjectCreatedEvent,
			OnGameObjectCopiedEvent,
			OnGameObjectDestroyedEvent,
			OnComponentCreatedEvent,
			OnComponentDestroyedEvent> EventData = {};
	};

}