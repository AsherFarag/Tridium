#pragma once
#include "entt.hpp"
using namespace entt::literals;

template<typename T>
struct Reflector
{
	Reflector()
	{
		T::OnReflect();
	}
};

#define REFLECT friend class SceneSerializer;\
				template<typename T>\
                friend class Reflector;\
                static void OnReflect();

#define _BEGIN_REFLECT_NO_BASE( Class )\
static Reflector<Class> s_Reflector##Class##Instance;\
void Class::OnReflect()\
{ \
using ClassType = Class; \
entt::meta<Class>().type( entt::type_hash<Class>::value() )

#define _BEGIN_REFLECT_CALL_BASE( Class, Base ) \
_BEGIN_REFLECT_NO_BASE(Class).base<Base>()

#define BEGIN_REFLECT(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, _BEGIN_REFLECT_CALL_BASE, _BEGIN_REFLECT_NO_BASE )(__VA_ARGS__))
	#define PROPERTY(Name) .data<&ClassType::Name>(entt::hashed_string( #Name ))
	#define FUNCTION(Name) .func<&ClassType::Name>(entt::hashed_string( #Name ))
#define END_REFLECT ; }