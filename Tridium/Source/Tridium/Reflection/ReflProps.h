#pragma once
#include "MetaFlags.h"
#include <entt.hpp>

namespace Tridium {

	// Forward Declarations
	class Component;
	class Scene;
	class ScriptEngine;
	// ---------------------

	namespace Refl {

		using MetaIDType = entt::id_type;

		// TypeInfo is a type that stores basic information about a type, such as it's name and identifier.
		// The identifier can be used to retrieve it's MetaType from the registry.
		using TypeInfo = entt::type_info;

		// A Property is a piece of data that is stored in a class.
		// Example: 
		// int MyInt
		// from
		// class MyClass { int MyInt; };
		using MetaProp = entt::meta_data; 

		// Func can be any type of function, including member functions and functors.
		using MetaFunc = entt::meta_func;

		// MetaAttribute is a property that is stored in a class's metadata.
		// It can be any type of data, and be retrieved at runtime to perform various operations.
		// This can be used to store additional information about a class that is not directly related to the class itself.
		using MetaAttribute = entt::meta_prop;

		// MetaAny acts like an std::any, but also stores the MetaType of the data it holds.
		using MetaAny = entt::meta_any;

		// MetaHandle stores a non-owning pointer to an object with it's associated MetaType.
		// It cannot perform copies and does not prolong the lifetime of the object it points to.
		using MetaHandle = entt::meta_handle;

		template<typename _Type, typename _Iterator>
		using MetaRange = entt::meta_range<_Type, _Iterator>;

		// = INTERNAL USE ONLY =
		// MetaFactory is used to construct the MetaType of a type.
		// It is used to register information about a type, such as it's properties and functions.
		template<typename T>
		using MetaFactory = entt::meta_factory<T>;
		template<typename T>
		MetaFactory<T> CreateMetaFactory() { return entt::meta<T>(); }



		// -- Function Signatures --
		typedef void			( *TextSerializeFunc )			( IO::Archive& a_Archive, const MetaAny& a_Data );
		typedef void			( *TextDeserializeFunc )		( const YAML::Node& a_Node, MetaAny& a_Data );
		typedef void			( *RegisterScriptableFunc )		( ScriptEngine& a_ScriptEngine );
		typedef Component*		( *AddToGameObjectFunc )		( Scene& a_Scene, GameObjectID a_GameObject );
		typedef void			( *RemoveFromGameObjectFunc )	( Scene& a_Scene, GameObjectID a_GameObject );

		#ifdef IS_EDITOR

		typedef bool ( *DrawPropertyFunc )( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags );

		#endif // IS_EDITOR
		// -------------------------



		// MetaData Properties are used to store additional information about a class that are not directly related to the class itself.
		// These properties can be retrieved via their ID, which is a hash of their name.
		// They can store any type of data, and be retrieved at runtime to perform various operations.
		namespace Props {

			template<MetaIDType _Name, typename _Type, bool _IsOverrideable = false>
			struct MetaDataProperty
			{
				using Type = _Type;
				static constexpr MetaIDType ID = _Name;
				static constexpr bool IsOverrideable = _IsOverrideable;
			};

			using namespace entt::literals;
			#define Internal( Name ) "_Internal_" Name##_hs

			// -- MetaData Properties --

			using CleanClassNameProp = MetaDataProperty<"_Internal_" "CleanClassName"_hs, std::string>;
			using ClassFlagsProp = MetaDataProperty<Internal( "ClassFlags" ), EClassFlags>;

			// -- Serialization Properties --

			using TextSerializeProp = MetaDataProperty<Internal( "TextSerialize" ), TextSerializeFunc, true>;
			using TextDeserializeProp = MetaDataProperty<Internal( "TextDeserialize" ), TextDeserializeFunc, true>;

			// -- GameObject Component Properties --

			using IsComponentProp = MetaDataProperty<Internal( "IsComponent" ), bool>;
			using AddToGameObjectProp = MetaDataProperty<Internal( "AddToGameObject" ), AddToGameObjectFunc>;
			using RemoveFromGameObjectProp = MetaDataProperty<Internal( "RemoveFromGameObject" ), RemoveFromGameObjectFunc>;

			// -- Scriptable Properties --

			using RegisterScriptableProp = MetaDataProperty<Internal("RegisterScriptable"), RegisterScriptableFunc>;

			// -- Editor-Only Properties --
			#ifdef IS_EDITOR

			using DrawPropertyProp = MetaDataProperty<Internal( "DrawProperty" ), DrawPropertyFunc, true>;

			#endif // IS_EDITOR



			#undef Internal

		}

	}

}