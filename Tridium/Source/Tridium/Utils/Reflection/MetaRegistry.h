#pragma once
#include <unordered_map>
#include "entt.hpp"
#include "yaml-cpp/yaml.h"


namespace Tridium::Refl {

	namespace Internal {

		// Serialize function signature typedef.
		typedef void ( *SerializeFunc )( YAML::Emitter& a_Out, const char* a_Name, const entt::meta_handle& a_Handle );

		// ID for the serialize function.
		constexpr entt::id_type YAMLSerializeFuncID = entt::hashed_string( "YAMLSerializeFuncID" ).value();
	}

	typedef entt::id_type MetaIDType;

	// Registry for meta data.
	class MetaRegistry
	{
	public:
		static MetaRegistry& Get();

		static void RegisterName( entt::id_type a_ID, const char* a_Name );

		static const char* GetName( const entt::id_type& a_ID );

		template<typename _Class, typename _MetaProperty>
		static bool TryGetMetaPropertyFromClass( _MetaProperty& o_Meta, MetaIDType a_MetaID );

		// TEMP

		// -------------------- Serialization --------------------

		template<typename T>
		static Internal::SerializeFunc GetSerializeFunction();

		template<typename T>
		static bool WriteObjectToEmitter( YAML::Emitter& a_Out, const char* a_Name, const T& a_Object );

		// -------------------------------------------------------

	#ifdef IS_EDITOR

	#endif // IS_EDITOR


	private:
		std::unordered_map<entt::id_type, const char*> s_Names;

		MetaRegistry() = default;
		MetaRegistry( const MetaRegistry& ) = delete;
		MetaRegistry& operator=( const MetaRegistry& ) = delete;
	};

}

#include "MetaRegistry.inl"