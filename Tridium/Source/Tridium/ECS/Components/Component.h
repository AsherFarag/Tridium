#pragma once
#include <type_traits>
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/GameObject.h>
#include "entt.hpp"
#include <Tridium/ECS/Reflection.h>

namespace Tridium {

	class Component
	{
		friend class GameObject;
	public:
		Component() {}
		virtual ~Component() = default;
		virtual void OnDestroy() {}

		const GameObject GetGameObject() const { return m_GameObject; }

	private:
		GameObject m_GameObject;
	};



#define DEFINE_BASE_COMPONENT( Name ) class Name : public Component
#define DEFINE_INHERITED_COMPONENT( Name, Base ) class Name : public Base

#define DEFINE_COMPONENT(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, DEFINE_INHERITED_COMPONENT, DEFINE_BASE_COMPONENT )(__VA_ARGS__))



	DEFINE_COMPONENT( ScriptableComponent )
	{
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