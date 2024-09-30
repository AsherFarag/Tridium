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
		REFLECT;
		friend class GameObject;
	public:
		Component() {}
		virtual ~Component() = default;
		virtual void OnDestroy() {}

		inline GameObject GetGameObject() const { return m_GameObject; }

	private:
		GameObject m_GameObject;
	};



#define DEFINE_BASE_COMPONENT( Name ) class Name : public Component
#define DEFINE_INHERITED_COMPONENT( Name, Base ) class Name : public Base

#define DEFINE_COMPONENT(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, DEFINE_INHERITED_COMPONENT, DEFINE_BASE_COMPONENT )(__VA_ARGS__))



	DEFINE_COMPONENT( ScriptableComponent )
	{
		REFLECT;
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