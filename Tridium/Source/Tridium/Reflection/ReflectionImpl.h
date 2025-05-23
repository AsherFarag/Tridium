#pragma once
#include "ReflectionFwd.h"
#include "MetaTypes.h"
#include <Tridium/Utils/Macro.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Component.h>
#include "EditorReflection.h"

#include <any>
#include <map>

// - SCRIPTABLE -
#include <Tridium/Scripting/ScriptEngine.h>
#include <sol/sol.hpp>

#define _HasFlag( Flags, Flag ) ( ( ( Flags ) & Flag ) == Flag )

namespace Tridium::Refl::Internal {

	// Property Flag Asserts
	template <EPropertyFlags _Flags>
	static constexpr void AssertPropertyFlags() 
	{
		static_assert( 
			!( _HasFlag( _Flags, EPropertyFlags::EditAnywhere ) && _HasFlag( _Flags, EPropertyFlags::VisibleAnywhere ) ),
			"Property cannot be both EditAnywhere and VisibleAnywhere." );

		static_assert(
			!( _HasFlag( _Flags, EPropertyFlags::ScriptReadOnly ) && _HasFlag( _Flags, EPropertyFlags::ScriptReadWrite ) ),
			"Property cannot be both ScriptReadOnly and ScriptReadWrite." );
	}

	// Helper Serialize Functions

	void SerializeClass( IO::Archive& a_Archive, const MetaAny& a_Data, const MetaType& a_MetaType );

	template <typename T>
	void SerializeClass( IO::Archive& a_Archive, const MetaAny& a_Data )
	{
		static const MetaType metaType = ResolveMetaType<T>();
		SerializeClass( a_Archive, a_Data, metaType );
	}

	void DeserializeClass( const YAML::Node& a_Node, MetaAny& a_Data, const MetaType& a_MetaType );

	template <typename T>
	void DeserializeClass( const YAML::Node& a_Node, MetaAny& a_Data )
	{
		static const MetaType metaType = ResolveMetaType<T>();
		DeserializeClass( a_Node, a_Data, metaType );
	}

} // namespace Tridium::Refl::Internal

#define _PROPERTY_FLAGS_ASSERT( Prop, Flags ) ::Tridium::Refl::Internal::AssertPropertyFlags<Flags>();



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OPT IN ATTRIBUTE MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// - SCRIPTABLE -

#define _DECLARE_SCRIPTABLE_PROPERTY_MAP \
	using RegisterScriptablePropertyFunc = void(*)( sol::usertype<ClassType>& a_Type ); \
	inline static std::map<std::string, RegisterScriptablePropertyFunc> s_ScriptPropertyMap;

#define _IS_SCRIPTABLE_PROPERTY(Prop, Flags) \
if constexpr ( _HasFlag( Flags, ::Tridium::Refl::EPropertyFlags::ScriptReadWrite ) ) \
{ \
	s_ScriptPropertyMap[#Prop] = +[]( sol::usertype<ClassType>& a_Type ) { a_Type[#Prop] = sol::property( &ClassType::Prop, &ClassType::Prop ); }; \
} \
else if constexpr ( _HasFlag( Flags, ::Tridium::Refl::EPropertyFlags::ScriptReadOnly ) ) \
{ \
	s_ScriptPropertyMap[#Prop] = +[]( sol::usertype<ClassType>& a_Type ) { a_Type[#Prop] = sol::property( &ClassType::Prop ); }; \
}

#define _ATTRIBUTE_REGISTER_SCRIPTABLE \
    static auto RegisterScriptableProperties = +[]( sol::usertype<ClassType>& a_UserType ) \
	{ \
		for ( const auto& [key, value] : s_ScriptPropertyMap ) \
		{ \
			( *value )( a_UserType ); \
		} \
	}; \
	factory.prop( ::Tridium::Refl::Props::RegisterScriptableProp::ID, +[]( ::Tridium::ScriptEngine& a_ScriptEngine ) \
		{ \
            sol::usertype<ClassType> type = a_ScriptEngine.RegisterNewType<ClassType>( ClassName() ); \
			RegisterScriptableProperties( type ); \
		} );

// ----------------

// - SERIALIZE -

#define _ATTRIBUTE_SERIALIZE \
    factory.prop( ::Tridium::Refl::Props::TextDeserializeProp::ID, +[](const YAML::Node& a_Node, MetaAny& a_Data) { DeserializeClass<ClassType>(a_Node, a_Data); } ); \
	factory.prop( ::Tridium::Refl::Props::TextSerializeProp::ID, +[](::Tridium::IO::Archive& a_Archive, const MetaAny& a_Data) { SerializeClass<ClassType>(a_Archive, a_Data); } );

// ----------------

// - DRAW PROPERTY -

#if WITH_EDITOR
#define _ATTRIBUTE_DRAW_PROPERTY \
	factory.prop( ::Tridium::Refl::Props::DrawPropertyProp::ID, \
		+[](const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags) \
			{ \
				return ::Tridium::Refl::Internal::DrawClassAsProperty<ClassType>(a_Name, a_Handle, a_Flags); \
			 } );
#else
#define _ATTRIBUTE_DRAW_PROPERTY
#endif // WITH_EDITOR

// ----------------

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BASE MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _BASE( Base ) \
 { factory.base<Base>(); }



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROPERTY MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _PROPERTY( Prop, Flags ) \
 { \
	_PROPERTY_FLAGS_ASSERT(Prop, Flags) \
	_IS_SCRIPTABLE_PROPERTY(Prop, Flags) \
	factory.data<&ClassType::Prop>( Hash( #Prop ), Flags, #Prop ); \
 }

#define _PROPERTY_NO_FLAGS( Prop ) _PROPERTY( Prop, ::Tridium::Refl::EPropertyFlags::EPF_None )

#define _PROPERTY_SELECTOR(...) EXPAND( SELECT_MACRO_2( __VA_ARGS__, _PROPERTY, _PROPERTY_NO_FLAGS )(__VA_ARGS__) )



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _FUNCTION( Func, Flags ) \
 { factory.func<&ClassType::Func>( Hash( #Func ) ); }

#define _FUNCTION_NO_FLAGS( Func ) _FUNCTION( Func, ::Tridium::Refl::EPropertyFlags::EPF_None )

#define _FUNCTION_SELECTOR(...) EXPAND( SELECT_MACRO_2( __VA_ARGS__, _FUNCTION, _FUNCTION_NO_FLAGS )(__VA_ARGS__) )



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// META MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _META( Key, Value ) \
 { factory.prop( Hash( Key ), Value ); }



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDE DEFAULTS MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _DECLARE_DEFAULT_ATTRIBUTE( NameSpace, MetaAttribute ) \
 std::optional<NameSpace::MetaAttribute::Type> MetaAttribute##_Override;

#define _DEFAULT_ATTRIBUTES() \
	_DECLARE_DEFAULT_ATTRIBUTE( ::Tridium::Refl::Props, TextSerializeProp ) \
	_DECLARE_DEFAULT_ATTRIBUTE( ::Tridium::Refl::Props, TextDeserializeProp ) \
    _DECLARE_DEFAULT_ATTRIBUTE_DRAW_PROPERTY \

#define _BIND_DEFAULT_ATTRIBUTE( NameSpace, MetaAttribute, DefaultValue ) \
 { \
	if ( MetaAttribute##_Override.has_value() ) { factory.prop( NameSpace::MetaAttribute::ID, MetaAttribute##_Override.value() ); } \
	else { factory.prop( NameSpace::MetaAttribute::ID, DefaultValue ); } \
 }

#define _BIND_DEFAULT_ATTRIBUTES() \
	_BIND_DEFAULT_ATTRIBUTE( ::Tridium::Refl::Props, TextSerializeProp, +[](::Tridium::IO::Archive& a_Archive, const MetaAny& a_Data) { SerializeClass<ClassType>(a_Archive, a_Data); } ) \
	_BIND_DEFAULT_ATTRIBUTE( ::Tridium::Refl::Props, TextDeserializeProp, +[](const YAML::Node& a_Node, MetaAny& a_Data) { DeserializeClass<ClassType>(a_Node, a_Data); } ) \
	_BIND_DEFAULT_ATTRIBUTE_DRAW_PROPERTY \



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OVERRIDE MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _OVERRIDE( MetaProp, Override ) \
 { \
    static_assert( MetaProp::IsOverrideable, "MetaProp is not overrideable." ); \
	/*static_assert( std::is_same_v<decltype(Override), MetaProp::Type>, "Override type doesn't match the MetaProp type." );*/ \
	factory.prop( MetaProp::ID, Override ); \
 }



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// END REFLECT MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _END_REFLECT( Class ) \
	} \
 }; static volatile ::Tridium::Refl::Internal::Reflector<Class> ___StaticInitializer_##Class;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN REFLECT MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _BEGIN_REFLECT( Class, Flags ) \
void Class::__Tridium_Reflect()  { LOG( LogCategory::Reflection, Trace, "Manually reflecting type '{0}'", #Class ); } \
template<> \
struct ::Tridium::Refl::Internal::Reflector<Class> \
{ \
	static constexpr MetaIDType Hash( const char* a_String ) { return entt::hashed_string::value( a_String ); } \
	static constexpr const char* ClassName() { return #Class; } \
	using ClassType = Class; \
	_DECLARE_SCRIPTABLE_PROPERTY_MAP \
	Reflector() \
	{ \
		using enum ::Tridium::Refl::EClassFlags; \
		using enum ::Tridium::Refl::EPropertyFlags; \
		MetaFactory factory = CreateMetaFactory<Class>(); \
		factory.type( Hash( #Class ) ); \
		factory.prop( Props::ClassFlagsProp::ID, Flags ); \
		factory.prop( Props::CleanClassNameProp::ID, #Class ); \
		_ATTRIBUTE_SERIALIZE \
        _ATTRIBUTE_DRAW_PROPERTY \
		_ATTRIBUTE_REGISTER_SCRIPTABLE \

#define _BEGIN_REFLECT_NO_FLAGS( Class ) _BEGIN_REFLECT( Class, ::Tridium::Refl::EClassFlags::ECF_None )

#define _BEGIN_REFLECT_SELECTOR(...) EXPAND( SELECT_MACRO_2( __VA_ARGS__, _BEGIN_REFLECT, _BEGIN_REFLECT_NO_FLAGS )(__VA_ARGS__) )



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN REFLECT COMPONENT MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _BEGIN_REFLECT_COMPONENT( Class, Flags ) \
 /*static_assert( std::is_base_of<::Tridium::Component, Class>::value, "Class doesn't derive from component! Use BEGIN_REFLECT instead. " );*/ \
 _BEGIN_REFLECT( Class, Flags ) \
 factory.prop( ::Tridium::Refl::Props::InitComponentProp::ID, +[]( ::Tridium::Scene& a_Scene ) { a_Scene.__InitComponentType<ClassType>(); } ); \
 factory.prop( ::Tridium::Refl::Props::AddToGameObjectProp::ID, +[](::Tridium::Scene& a_Scene, ::Tridium::EntityID a_GameObject) -> ::Tridium::Component* { return a_Scene.TryAddComponentToGameObject<ClassType>(::Tridium::GameObject{a_GameObject}); } ); \
 factory.prop( ::Tridium::Refl::Props::RemoveFromGameObjectProp::ID, +[](::Tridium::Scene& a_Scene, ::Tridium::EntityID a_GameObject) { a_Scene.RemoveComponentFromGameObject<ClassType>(::Tridium::GameObject{a_GameObject}); } ); \
 factory.prop( Props::IsComponentProp::ID, true ); \
 static auto RegisterScriptableComponent = +[]( sol::usertype<Class>& a_UserType ) \
 { \
	 RegisterScriptableProperties( a_UserType ); \
	 a_UserType["gameObject"] = sol::property( &Class::GetGameObject ); \
	 a_UserType["AddToGameObject"] = +[]( GameObject a_GameObject ) -> Class* { return a_GameObject.TryAddComponent<Class>(); }; \
	 a_UserType["RemoveFromGameObject"] = +[]( GameObject a_GameObject ) { a_GameObject.RemoveComponent<Class>(); }; \
 }; \
 factory.prop( Props::RegisterScriptableProp::ID, +[]( ::Tridium::ScriptEngine& a_ScriptEngine ) \
	{ \
        sol::usertype<Class> type = a_ScriptEngine.RegisterNewType<Class>( ClassName() ); \
		RegisterScriptableComponent( type ); \
	} ); \

#define _BEGIN_REFLECT_COMPONENT_NO_FLAGS( Class ) _BEGIN_REFLECT_COMPONENT( Class, ::Tridium::Refl::EClassFlags::ECF_None )

#define _BEGIN_REFLECT_COMPONENT_SELECTOR(...) EXPAND( SELECT_MACRO_2( __VA_ARGS__, _BEGIN_REFLECT_COMPONENT, _BEGIN_REFLECT_COMPONENT_NO_FLAGS )(__VA_ARGS__) )



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// END REFLECT COMPONENT MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _END_REFLECT_COMPONENT( Class ) \
 _END_REFLECT( Class )
	//	_BIND_DEFAULT_ATTRIBUTES(); \
	//} \
 //}; static volatile ::Tridium::Refl::Internal::Reflector<Class> ___StaticInitializer_##Class; \
//static_assert( std::is_base_of<::Tridium::Component, Class>::value, "Class doesn't derive from component! Use END_REFLECT instead. " );



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN REFLECT ENUM MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _BEGIN_REFLECT_ENUM( Enum ) \
template<> \
struct ::Tridium::Refl::Internal::Reflector<Enum> \
{ \
	static constexpr MetaIDType Hash( const char* a_String ) { return entt::hashed_string::value( a_String ); } \
	using ClassType = Enum; \
	Reflector() \
	{ \
		using enum ::Tridium::Refl::EClassFlags; \
		using enum ::Tridium::Refl::EPropertyFlags; \
		MetaFactory factory = CreateMetaFactory<Enum>(); \
		factory.type( Hash( #Enum ) ); \
		factory.prop( Props::ClassFlagsProp::ID, EClassFlags::ECF_None ); \
		factory.prop( Props::CleanClassNameProp::ID, #Enum ); \
		factory.prop( Props::DrawPropertyProp::ID, _DRAW_ENUM_FUNC(Enum) ); \
		factory.prop(Props::TextSerializeProp::ID,                                     \
              +[](::Tridium::IO::Archive& a_Archive, const MetaAny& a_Data)            \
              {                                                                        \
                  a_Archive <<  s_EnumToString.at(a_Data.cast<ClassType>());           \
              });                                                                      \
		factory.prop( Props::TextDeserializeProp::ID,                                  \
              +[](const YAML::Node& a_Node, MetaAny& a_Data)                           \
              {                                                                        \
                  std::string enumString = a_Node.as<std::string>();                   \
				  for (auto&& [enumValue, enumStr] : s_EnumToString)                   \
				  {                                                                    \
					  if (enumStr == enumString)                                       \
					  {                                                                \
						  a_Data.cast<ClassType&>() = enumValue;                       \
						  break;                                                       \
					  }                                                                \
				  }                                                                    \
              });



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ENUM VALUE MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _ENUM_VALUE(EnumVal) \
    constexpr entt::hashed_string Hashed_##EnumVal(#EnumVal); \
    factory.data<ClassType::EnumVal>(Hashed_##EnumVal, \
                                  ::Tridium::Refl::EPropertyFlags::EPF_None, \
                                  #EnumVal); \
    s_EnumToString.insert({ClassType::EnumVal, #EnumVal});



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// END REFLECT ENUM MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _END_REFLECT_ENUM( Enum ) \
     	factory.prop( ::Tridium::Refl::Props::DrawPropertyProp::ID, _DRAW_ENUM_FUNC(Enum) ); \
	} \
	inline static std::unordered_map<Enum, std::string> s_EnumToString; \
 }; static volatile ::Tridium::Refl::Internal::Reflector<Enum> ___StaticInitializer_##Enum;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////