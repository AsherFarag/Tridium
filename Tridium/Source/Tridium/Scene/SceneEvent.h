#pragma once
#include <Tridium/Core/Types.h>
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
		OnEnd,

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
		GameObjectID GameObjectID = INVALID_GAMEOBJECT_ID;
	};

	struct OnGameObjectCopiedEvent
	{
		GameObjectID SourceID = INVALID_GAMEOBJECT_ID;
		GameObjectID DestinationID = INVALID_GAMEOBJECT_ID;
	};

	struct OnGameObjectDestroyedEvent
	{
		GameObjectID GameObjectID = INVALID_GAMEOBJECT_ID;
	};

	struct OnComponentCreatedEvent
	{
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