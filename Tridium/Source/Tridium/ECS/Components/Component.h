#pragma once
#include <type_traits>
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/GameObject.h>
#include "entt.hpp"

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


namespace Tridium {

	struct Reflector
	{
		template < auto _Member >
		void AddMember( const char* a_Name )
		{

		}

		template < auto _Function >
		void AddFunction( const char* a_Name )
		{
			using ObjectType = typename ReflectorHelper< decltype( _Member ) >::ObjectType;

			entt::meta< ObjectType >().func< _Function >( a_Name );
		}
	};

	class Component;
#define DEFINE_BASE_COMPONENT( Name ) class Name : public Component
#define DEFINE_INHERITED_COMPONENT( Name, Base ) class Name : public Base
#define DEFINE_COMPONENT(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, DEFINE_INHERITED_COMPONENT, DEFINE_BASE_COMPONENT )(__VA_ARGS__))

#define DEFINE_COMPONENT_BODY_BASE(Name) \
	using BaseType = Component; \
	const BaseType* GetBase() const { return (const BaseType*)this; } \
	BaseType* GetBase() { return (BaseType*)this; }

#define DEFINE_COMPONENT_BODY_INHERITED(Name, Base) \
	using BaseType = Base; \
	const BaseType* GetBase() const { return (const BaseType*)this; } \
	BaseType* GetBase() { return (BaseType*)this; }

#define DEFINE_COMPONENT_BODY(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, DEFINE_COMPONENT_BODY_INHERITED, DEFINE_COMPONENT_BODY_BASE )(__VA_ARGS__))

	class Component
	{
		DEFINE_COMPONENT_BODY(Component)

		friend class GameObject;
	public:
		Component() {}
		virtual ~Component() = default;

		const GameObject GetGameObject() const { return m_GameObject; }

		static void DeclareUserType() {}
		static void OnReflect( Reflector& reflector );
	private:
		GameObject m_GameObject;
	};

	DEFINE_COMPONENT( ScriptableComponent )
	{
		friend class Scene;
		friend class GameObject;
	public:
		DEFINE_COMPONENT_BODY( ScriptableComponent );

		ScriptableComponent() {}
		virtual ~ScriptableComponent();

		virtual void OnUpdate() {}

	protected:
		virtual void OnConstruct() {}
		virtual void OnDestroy() {}
	};
}


//template <>
//struct PropertyDrawer<Tridium::ScriptableComponent>
//{
//	void OnImGui() override
//	{
//
//	}
//};