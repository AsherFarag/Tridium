#pragma once
#include <Tridium/Utils/Macro.h>
#include <Tridium/Utils/Reflection/MetaRegistry.h>
#include "ReflectionFwd.h"

#include "EditorReflection.h"

namespace Tridium
{
    class GameObject;

    namespace Refl {

        // Used to statically reflect a class.
        template<typename T>
        struct Reflector;

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
		using ClassType = Class;						  \
	    Reflector()                                       \
	    {												  \
            using enum _REFL_ EPropertyFlag;              \
	        using namespace entt::literals;				  \
            auto meta = entt::meta<Class>();              \
			meta.type( entt::type_hash<Class>::value() ); \
            meta.prop( _REFL_ Internal::CleanClassNamePropID, #Class );      

namespace Tridium::Refl::Internal {

	void SerializeClass( IO::Archive& a_Archive, const MetaAny& a_Data, const MetaType& a_MetaType );

	template <typename T>
	void SerializeClass( IO::Archive& a_Archive, const MetaAny& a_Data )
	{
		static const MetaType metaType = MetaRegistry::ResolveMetaType<T>();
		SerializeClass( a_Archive, a_Data, metaType );
	}
}

// Add a serialize static lambda function to the meta data.
#define _REFLECT_SERIALIZE_HELPER(Class)                                                       \
    meta.prop(_REFL_ Internal::YAMLSerializeFuncID,                                            \
              +[](::Tridium::IO::Archive& a_Archive, const _REFL_ MetaAny& a_Data)             \
		      {                                                                                \
				  _REFL_ Internal::SerializeClass<Class>(a_Archive, a_Data);                   \
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
#define END_REFLECT(Class) } }; ::Tridium::Refl::Reflector<Class> Class::___StaticInitializer_##Class;

// This macro is used to define the flags for a property.
// Example: PROPERTY( Name, FLAGS( Serialize, VisibleAnywhere ) )
#define FLAGS(...) _REFL_ Internal::CombineFlags(__VA_ARGS__)

#define BEGIN_REFLECT_COMPONENT(ComponentClass)                     \
    BEGIN_REFLECT(ComponentClass)                                   \
    meta.prop(::Tridium::Refl::Internal::IsComponentPropID);	    \
    meta.prop(::Tridium::Refl::Internal::AddToGameObjectPropID,     \
			  +[](GameObject a_GameObject)                          \
			  {                                                     \
				  a_GameObject.TryAddComponent<ComponentClass>();   \
			  });                                                   