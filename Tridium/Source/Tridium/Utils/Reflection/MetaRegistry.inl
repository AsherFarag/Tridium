#include "MetaRegistry.h"

namespace Tridium::Refl {

	inline MetaRegistry& MetaRegistry::Get()
	{
		static MetaRegistry s_Instance;
		return s_Instance;
	}

	inline void MetaRegistry::RegisterName( MetaIDType a_ID, const char* a_Name )
	{
		Get().s_Names[a_ID] = a_Name;
	}

	inline const char* MetaRegistry::GetName( MetaIDType a_ID )
	{
		auto it = Get().s_Names.find( a_ID );
		if ( it == Get().s_Names.end() )
			return "Unknown";

		return it->second;
	}

	inline MetaType MetaRegistry::GetMetaType( MetaIDType a_ID )
	{
		return entt::resolve(a_ID);
	}

	template<typename _Class, typename _MetaProperty>
	inline bool MetaRegistry::TryGetMetaPropertyFromClass( _MetaProperty& o_Meta, MetaIDType a_MetaID )
	{
		static const auto meta = entt::resolve<_Class>();
		if ( auto prop = meta.prop( a_MetaID ) )
		{
			if ( prop.value().try_cast<_MetaProperty>( ) )
			{
				o_Meta = prop.value().cast<_MetaProperty>();
				return true;
			}
		}

		return false;
	}

	template<typename T>
	inline Internal::SerializeFunc MetaRegistry::GetSerializeFunction()
	{
		auto meta = entt::resolve<T>();
		// Check if the class has a serialize function.
		if ( auto serializeFunc = meta.prop( Internal::YAMLSerializeFuncID ) )
		{
			if ( serializeFunc.value().try_cast<Internal::SerializeFunc>( ) )
			{
				return serializeFunc.value().cast<Internal::SerializeFunc>();
			}
		}

		return nullptr;
	}

	template<typename T>
	inline bool MetaRegistry::WriteObjectToEmitter( YAML::Emitter& a_Out, const char* a_Name, const T& a_Object )
	{
		if ( Internal::SerializeFunc serFunc = GetSerializeFunction<T>() )
		{
			a_Out << YAML::BeginMap;
			serFunc( a_Out, a_Name, entt::meta_handle( a_Object ) );
			a_Out << YAML::EndMap;
			return true;
		}

		return false;
	}

} // namespace Tridium::Reflection