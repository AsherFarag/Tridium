#pragma once
#include <unordered_map>
#include "entt.hpp"
#include "yaml-cpp/yaml.h"
#include "Meta.h"


namespace Tridium::Refl {

	namespace Internal {

		// Serialize function signature typedef.
		typedef void ( *SerializeFunc )( YAML::Emitter& a_Out, const char* a_Name, const entt::meta_handle& a_Handle );

		// ID for the serialize function.
		constexpr MetaIDType YAMLSerializeFuncID = entt::hashed_string( "YAMLSerializeFuncID" ).value();
	}

	class MetaRegistry final
	{
	public:
		static MetaRegistry& Get();

		static inline MetaType ResolveMetaType( const TypeInfo& a_TypeInfo );

		static inline MetaType ResolveMetaType( MetaIDType a_ID );

		template<typename T>
		static MetaType ResolveMetaType();

		template<typename _MetaProperty>
		static bool TryGetMetaPropertyFromClass( MetaType a_ClassID, _MetaProperty& o_Meta, MetaIDType a_MetaID );

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
		MetaRegistry() = default;
		MetaRegistry( const MetaRegistry& ) = delete;
		MetaRegistry& operator=( const MetaRegistry& ) = delete;
	};

}

#include "MetaRegistry.inl"