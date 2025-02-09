#pragma once
#include "MetaFlags.h"
#include <Tridium/ECS/ECS.h>
#include "ReflectionFwd.h"

namespace Tridium {

	// Forward Declarations
	class Scene;
	class ScriptEngine;
	class Component;
	class GameObject;

	namespace Script {
		class ScriptEngine;
	}
	// ---------------------

	namespace Refl {

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
		using TextSerializeFunc				= void (*)( IO::Archive& a_Archive, const MetaAny& a_Data );
		using TextDeserializeFunc			= void (*)( const YAML::Node& a_Node, MetaAny& a_Data );
		using RegisterScriptableFunc		= void (*)( ScriptEngine& a_ScriptEngine );
		using AddToGameObjectFunc			= Component* (*)( Scene& a_Scene, EntityID a_GameObject );
		using RemoveFromGameObjectFunc		= void (*)( Scene& a_Scene, EntityID a_GameObject );
		using InitComponentFunc				= void (*)( Scene& a_Scene );


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

			using CleanClassNameProp = MetaDataProperty<Internal( "CleanClassName" ), std::string>;
			using ClassFlagsProp = MetaDataProperty<Internal( "ClassFlags" ), EClassFlags>;

			// -- Serialization Properties --

			using TextSerializeProp = MetaDataProperty<Internal( "TextSerialize" ), TextSerializeFunc, true>;
			using TextDeserializeProp = MetaDataProperty<Internal( "TextDeserialize" ), TextDeserializeFunc, true>;

			// -- GameObject Component Properties --

			using InitComponentProp = MetaDataProperty<Internal( "InitComponent" ), InitComponentFunc>;
			using IsComponentProp = MetaDataProperty<Internal( "IsComponent" ), bool>;
			using AddToGameObjectProp = MetaDataProperty<Internal( "AddToGameObject" ), AddToGameObjectFunc>;
			using RemoveFromGameObjectProp = MetaDataProperty<Internal( "RemoveFromGameObject" ), RemoveFromGameObjectFunc>;

			// -- Scriptable Properties --

			using RegisterScriptableProp = MetaDataProperty<Internal("RegisterScriptable"), RegisterScriptableFunc, true>;

			// -- Editor-Only Properties --
			#ifdef IS_EDITOR

			using DrawPropertyProp = MetaDataProperty<Internal( "DrawProperty" ), DrawPropertyFunc, true>;

			#endif // IS_EDITOR



			#undef Internal

		}

	}

}