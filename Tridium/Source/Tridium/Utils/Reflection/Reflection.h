#pragma once
#include "ReflectionFwd.h"
#include <unordered_map>
#include "entt.hpp"
#include "yaml-cpp/yaml.h"

namespace Tridium::Reflection {

	constexpr entt::id_type _Internal_YAML_OnSerialize_ID = entt::hashed_string( "Internal_YAML_OnSerialize" ).value();

	typedef void ( *SerializeFunc )( YAML::Emitter& a_Out, const char* a_Name, const entt::meta_handle& a_Handle );

	template<typename T>
	struct Reflector
	{
		Reflector()
		{
			T::OnReflect();
		}
	};

	class MetaRegistry
	{
	public:
		static MetaRegistry& Get();

		static void RegisterName( entt::id_type a_ID, const char* a_Name )
		{
			Get().s_Names[a_ID] = a_Name;
		}

		static const char* GetName( const entt::id_type& a_ID )
		{
			auto it = Get().s_Names.find( a_ID );
			if ( it == Get().s_Names.end() )
				return "Unknown";

			return it->second;
		}

		template<typename T>
		static bool WriteObjectToEmitter( YAML::Emitter& a_Out, const char* a_Name, const T& a_Object )
		{
			auto meta = entt::resolve<T>();
			if ( auto serializeFunc = meta.prop( _Internal_YAML_OnSerialize_ID ) )
			{
				if ( serializeFunc.value().try_cast<SerializeFunc>( ) )
				{
					a_Out << YAML::BeginMap;
					serializeFunc.value().cast<SerializeFunc>()( a_Out, a_Name, entt::meta_handle( a_Object ));
					a_Out << YAML::EndMap;
					return true;
				}
			}

			return false;
		}

	private:
		std::unordered_map<entt::id_type, const char*> s_Names;

		MetaRegistry() = default;
		MetaRegistry( const MetaRegistry& ) = delete;
		MetaRegistry& operator=( const MetaRegistry& ) = delete;
	};
} // namespace Tridium::Reflection

// Reflection macros

#define _BEGIN_REFLECT_BODY_HELPER( Class )                                      \
    static ::Tridium::Reflection::Reflector<Class> s_Reflector##Class##Instance; \
    void Class::OnReflect()                                                      \
    {                                                                            \
    using ClassType = Class;                                                     \
    auto meta = entt::meta<Class>();                                             \
    meta.type( entt::type_hash<Class>::value() );

#define _REFLECT_SERIALIZE_MEMBERS_HELPER(Meta, ClassData)                                                                \
    for (auto&& [id_unique, data_unique] : Meta.data()) 															      \
	{                 																				                      \
        if (auto serializeFunc = data_unique.type().prop(::Tridium::Reflection::_Internal_YAML_OnSerialize_ID))           \
		{              																			                          \
			auto name = ::Tridium::Reflection::MetaRegistry::GetName(id_unique);                                          \
            serializeFunc.value().cast<::Tridium::Reflection::SerializeFunc>()(a_Out, name, data_unique.get(*ClassData)); \
        }                                                                                                                 \
		else 																										      \
		{                                                                                                                 \
			auto name = ::Tridium::Reflection::MetaRegistry::GetName(id_unique);                                          \
			a_Out << YAML::Key << name;                                                                                   \
			a_Out << YAML::Value << *static_cast<const int64_t*>( data_unique.get( *ClassData ).data() );                 \
		}                                                                                                                 \
    }

// Add a serialize function to the meta data
#define _REFLECT_SERIALIZE_HELPER(Class)                                                       \
    meta.prop(::Tridium::Reflection::_Internal_YAML_OnSerialize_ID,                            \
              +[](YAML::Emitter& a_Out, const char* a_Name, const entt::meta_handle& a_Handle) \
		      {                                                                                \
                  using ClassT = Class;                                                        \
                  auto classData = static_cast<const ClassT*>(a_Handle->data());               \
                  auto meta = entt::resolve<ClassT>();                                         \
                  a_Out << YAML::Key << a_Name;                                                \
                  a_Out << YAML::Value;                                                        \
                  a_Out << YAML::BeginMap;                                                     \
                  /* Serialize parent classes */                                               \
                  for (auto&& [id, base] : meta.base())                                        \
				  {                                                                            \
                      _REFLECT_SERIALIZE_MEMBERS_HELPER( base, classData)                      \
                  }                                                                            \
                  /* Serialize members */                                                      \
                  _REFLECT_SERIALIZE_MEMBERS_HELPER(meta, classData)                           \
				  a_Out << YAML::EndMap;                                                       \
              });

#define _REFLECT_MEMBER_HELPER( Name, Type )                                                  \
    ::Tridium::Reflection::MetaRegistry::RegisterName( entt::hashed_string( #Name ), #Name ); \
    meta.Type<&ClassType::Name>(entt::hashed_string( #Name ));

// To be defined in the class implementation
#define BEGIN_REFLECT(Class)          \
    _BEGIN_REFLECT_BODY_HELPER(Class) \
	_REFLECT_SERIALIZE_HELPER(Class)

    #define BASE(Base) meta.base<Base>();
    #define PROPERTY(Name) _REFLECT_MEMBER_HELPER(Name, data)
	#define FUNCTION(Name) _REFLECT_MEMBER_HELPER(Name, func)
#define END_REFLECT ; }