#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/GameObject.h>
#include "entt.hpp"

namespace Tridium {

	class Component
	{
		friend class GameObject;
	public:
		Component() {}
		virtual ~Component() = default;

		const GameObject GetGameObject() const { return m_GameObject; }

	private:
		GameObject m_GameObject;
	};

#define DEFINE_BASE_COMPONENT( x ) class x : public Component
#define DEFINE_INHERITED_COMPONENT( x, y ) class x : public y

#define DEFINE_COMPONENT(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, DEFINE_INHERITED_COMPONENT, DEFINE_BASE_COMPONENT )(__VA_ARGS__))

	DEFINE_COMPONENT( ScriptableComponent )
	{
		friend class Scene;
		friend class GameObject;
	public:
		ScriptableComponent() {}
		~ScriptableComponent();

		virtual void OnUpdate() {}

	protected:
		virtual void OnConstruct() {}
		virtual void OnDestroy() {}
	};
}