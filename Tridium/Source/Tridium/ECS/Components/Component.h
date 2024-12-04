#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/GameObject.h>
#include "entt.hpp"
#include <Tridium/Reflection/ReflectionFwd.h>

namespace Tridium {

	class Component
	{
		REFLECT( Component );
		friend class Scene;
		friend class GameObject;
	public:
		Component();
		virtual ~Component() = default;
		virtual void OnDestroy() {}

		inline GameObject GetGameObject() const { return m_GameObject; }

	private:
		GameObject m_GameObject;
	};

	class ScriptableComponent : public Component
	{
		REFLECT( ScriptableComponent );
		friend class Scene;
		friend class GameObject;

	public:
		ScriptableComponent() {}
		virtual ~ScriptableComponent() = default;
		inline SharedPtr<Scene> GetScene() const { return Application::GetScene(); }

		virtual void OnBeginPlay() {}
		virtual void OnUpdate() {}
		virtual void OnDestroy() {}
	};
}