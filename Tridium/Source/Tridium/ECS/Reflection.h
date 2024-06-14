#pragma once
#include "entt.hpp"
using namespace entt::literals;

#define REFLECT friend class Reflector; static void OnReflect()
#define START_REFLECT(Class) void Class::OnReflect() { entt::meta<Class>().type(entt::type_hash<Class>::value())
#define PROPERTY(Class, Name) .data<&Class::Name>(entt::hashed_string( #Name ))
#define FUNCTION(Class, Name) .func<&Class::Name>(entt::hashed_string( #Name ))
#define END_REFLECT ; }