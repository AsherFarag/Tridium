#pragma once
#include <type_traits>
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/GameObject.h>
#include "entt.hpp"
#include <Tridium/ECS/Reflection.h>

//struct IPropertyDrawer
//{
//	virtual void OnImGui()
//	{
//
//	}
//};
//
//template < typename T >
//struct PropertyDrawer
//{
//	void OnImGui() override
//	{
//		 entt::resolve< T >().fields();
//		 for ( field : fields )
//		 {}
//	}
//};
//template <>
//struct PropertyDrawer<Tridium::ScriptableComponent>
//{
//	void OnImGui() override
//	{
//
//	}
//};


namespace Tridium {

	class Component
	{
		friend class GameObject;
	public:
		Component() {}
		virtual ~Component() = default;

		const GameObject GetGameObject() const { return m_GameObject; }
		static void OnReflect();

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
		virtual ~ScriptableComponent();

		virtual void OnUpdate() {}

	protected:
		virtual void OnConstruct() {}
		virtual void OnDestroy() {}
	};
}