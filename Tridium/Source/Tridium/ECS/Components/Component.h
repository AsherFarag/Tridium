#pragma once
#include <Tridium/Utils/Reflection/Reflection.h>
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/GameObject.h>
#include "entt.hpp"
#include <Tridium/Utils/Reflection/ReflectionFwd.h>

namespace Tridium {

	namespace Refl {
		constexpr MetaIDType IsComponentID = entt::hashed_string( "IsComponent" ).value();
	}

	class Component
	{
		REFLECT( Component )
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
		virtual void OnDestroy() {}

		virtual void OnUpdate() {}

	protected:
		virtual void OnConstruct() {}
	};
}