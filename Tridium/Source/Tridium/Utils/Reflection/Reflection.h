#pragma once
#include <Tridium/Utils/Macro.h>
#include <Tridium/Utils/Reflection/MetaRegistry.h>
#include "ReflectionFwd.h"

#include "EditorReflectionMacros.h"

namespace Tridium
{
    class GameObject;

    namespace Refl {

        // Used to statically reflect a class.
        template<typename T>
        struct Reflector { };

        namespace Internal {

            // Helper function for combining flags using fold expression
            template<typename... EnumType>
            constexpr auto CombineFlags( EnumType... flags )
            {
                return ( static_cast<std::underlying_type_t<EnumType>>( flags ) | ... );
            }

            constexpr MetaIDType IsComponentPropID = entt::hashed_string( "IsComponent" ).value();
            constexpr MetaIDType AddToGameObjectPropID = entt::hashed_string( "AddToGameObject" ).value();
            typedef void( *AddToGameObjectFunc )( ::Tridium::GameObject a_GameObject );
        }

        void __Internal_InitializeReflection();
    }

}

#pragma region Internal Reflection Macro Helpers

#define _REFL_ ::Tridium::Refl::

// Serialize function signature typedef
#define _BEGIN_REFLECT_BODY_HELPER( Class )               \
    template<>											  \
    struct _REFL_ Reflector<Class>			              \
    {													  \
	    static Reflector<Class> s_Reflector##Class;       \
		using ClassType = Class;						  \
	    Reflector()                                       \
	    {												  \
	        using namespace entt::literals;				  \
            auto meta = entt::meta<Class>();              \
            meta.type( entt::type_hash<Class>::value() ); \
            meta.prop( "CleanClassName"_hs, #Class );     \
			using enum _REFL_ EPropertyFlag;              


// Serialize all members of a class.
#define _REFLECT_SERIALIZE_MEMBERS_HELPER(Meta, ClassData)                                                                                            \
    for (auto&& [id_unique, data_unique] : Meta.data()) 															                                  \
	{                 																				                                                  \
		if  ( !HasFlag( data_unique.propFlags(), EPropertyFlag::Serialize ) )                                                                         \
		{                                                                                                                                             \
			continue;                                                                                                                                 \
		}                                                                                                                                             \
        if (auto serializeFunc = data_unique.type().prop( _REFL_ Internal::YAMLSerializeFuncID))                                                      \
		{              																			                                                      \
            serializeFunc.value().cast<_REFL_ Internal::SerializeFunc>()(a_Out, data_unique.name().c_str(), data_unique.get(*ClassData));             \
        }                                                                                                                                             \
		else 																										                                  \
		{                                                                                                                                             \
			a_Out << YAML::Key << data_unique.name().c_str();                                                                                         \
			a_Out << YAML::Value << std::string("Refl Error: No serialize function for type <") + data_unique.type().info().name().data(), + ">!"; \
		}                                                                                                                                             \
    }



// Add a serialize static lambda function to the meta data.
#define _REFLECT_SERIALIZE_HELPER(Class)                                                       \
    meta.prop(_REFL_ Internal::YAMLSerializeFuncID,                                            \
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



#define _REFLECT_REGISTER_NAME( Name ) constexpr entt::hashed_string Hashed_##Name ( #Name ); _REFL_ MetaRegistry::RegisterName( Hashed_##Name, #Name );

#define _REFLECT_FUNCTION(Name) constexpr entt::hashed_string Hashed_##Name ( #Name ); meta.func<&ClassType::Name>(Hashed_##Name);	

// Used in a macro selector
// Define a property with flags.
#define _REFLECT_PROPERTY_FLAGS( Name, Flags ) constexpr entt::hashed_string Hashed_##Name( #Name ); meta.data<&ClassType::Name>(Hashed_##Name, static_cast<_REFL_ PropertyFlags>( Flags ), #Name);

// Used in a macro selector
// Define a property with no flags.
#define _REFLECT_PROPERTY_NO_FLAGS( Name ) _REFLECT_PROPERTY_FLAGS( Name, _REFL_ EPropertyFlag::None )

// Used in a macro selector
// Define a meta property with a value.
#define _REFLECT_META_VALUE( Name, Value ) meta.prop(Name##_hs, Value);

// Used in a macro selector
// Define a meta property with no value.
#define _REFLECT_META_NO_VALUE( Name ) meta.prop(Name##_hs);

#pragma endregion

// ----- Reflection macros ----- //
// To be defined in the class implementation.

// Example:
// 
// In the header file:
// 
// class MyStruct
// {
// 	REFLECT;
// 	int MyInt = 5;
//  void MyFunction();
// };
//
// In the source file:
// 
// BEGIN_REFLECT( MyStruct )
//  PROPERTY( MyInt, FLAGS( Serialize, VisibleAnywhere ) )
//  FUNCTION( MyFunction )
// END_REFLECT( MyStruct )

// This macro is used to define the reflection data for a class.
#define BEGIN_REFLECT(Class) _BEGIN_REFLECT_BODY_HELPER(Class) _REFLECT_SERIALIZE_HELPER(Class) _DRAW_PROPERTY_FUNC(Class)
	// Defines a base class this class inherits from
    #define BASE(Base) meta.base<Base>();
    // Adds user-defined bit flag traits to the type.
    #define TRAITS(...) TO BE IMPLEMENTED   //meta.traits(_REFL_ Internal::CombineFlags(__VA_ARGS__));
	// Allows user defined meta properties and functions that are not apart of the class.
    #define META(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, _REFLECT_META_VALUE, _REFLECT_META_NO_VALUE)(__VA_ARGS__))
	// Defines the properties that are apart of the class.
    #define PROPERTY(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, _REFLECT_PROPERTY_FLAGS, _REFLECT_PROPERTY_NO_FLAGS )(__VA_ARGS__))
	// Defines the functions that are apart of the class.
    #define FUNCTION(Name) _REFLECT_FUNCTION(Name)
// Ends the reflection data for a class.
#define END_REFLECT(Class) } }; static _REFL_ Reflector<Class> s_Reflector##Class;

// This macro is used to define the flags for a property.
// Example: PROPERTY( Name, FLAGS( Serialize, VisibleAnywhere ) )
#define FLAGS(...) _REFL_ Internal::CombineFlags(__VA_ARGS__)

#define BEGIN_REFLECT_COMPONENT(ComponentClass)                     \
    BEGIN_REFLECT(ComponentClass)                                   \
    meta.prop(::Tridium::Refl::Internal::IsComponentPropID);	    \
    meta.prop(::Tridium::Refl::Internal::AddToGameObjectPropID,     \
			  +[](GameObject a_GameObject)                          \
			  {                                                     \
				  a_GameObject.AddComponent<ComponentClass>();      \
			  });                                                   