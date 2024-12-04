#include "tripch.h"
#include "ScriptEngine.h"

#include "Tridium/Asset/AssetManager.h"
#include <Tridium/ECS/Components/Types.h>

namespace Tridium {

	UniquePtr<ScriptEngine> ScriptEngine::s_Instance = nullptr;

	void ScriptEngine::Init()
	{
		// Open the standard libraries
		m_LuaState.open_libraries( sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::os, sol::lib::io );

		// Register reflected types into the Lua state
		//for ( const auto&& [id, type] : Refl::MetaRegistry::ResolveMetaTypes() )
		//{
		//	Refl::MetaType metaType = type;
		//	if ( !metaType.IsValid() )
		//		continue;

		//	if ( auto registerScriptable = metaType.Prop( Refl::Internal::RegisterScriptablePropID ) )
		//	{
		//		registerScriptable.value().cast<Refl::Internal::RegisterScriptableFunc>()( *this );
		//	}
		//}
	}

	void ScriptEngine::RegisterType( const char* a_TypeName, Refl::MetaType a_Type )
	{
		auto typeTable = m_LuaState[a_TypeName];
		if ( !typeTable.valid() )
		{
			TE_CORE_ASSERT( false, "Failed to register type in Lua" );
			return;
		}

		// Register the type's properties
		for ( const auto& [id, prop] : a_Type.Properties() )
		{
			typeTable[prop.name()] = sol::property( &Vector3::x );
		}
	}

	bool ScriptEngine::RecompileScript( Script& a_Script )
	{
		// Clear the current environment
		a_Script.m_Environment = {};
		a_Script.m_Variables.clear();
		a_Script.m_Functions.clear();

		a_Script.m_LoadResult = Get().m_LuaState.load( a_Script.m_Source );
		if ( !a_Script.m_LoadResult.valid() )
		{
			sol::error err = a_Script.m_LoadResult;
			a_Script.m_CompileErrorMsg = err.what();
			TE_CORE_ERROR( "SCRIPT COMPILATION ERROR: {0}", a_Script.m_CompileErrorMsg );
			return false;
		}

		a_Script.m_LoadResult( a_Script.m_Environment );

		// Extract the variables and functions from the environment and store them in the script
		for ( const auto& [key, value] : a_Script.m_Environment )
		{
			std::string name = key.as<std::string>();

			switch ( value.get_type() )
			{
				case sol::type::boolean:
				{
					a_Script.m_Variables[name] = ScriptVariable( value.as<bool>() );
					break;
				}
				case sol::type::number:
				{
					a_Script.m_Variables[name] = ScriptVariable( value.as<double>() );
					break;
				}
				case sol::type::string:
				{
					a_Script.m_Variables[name] = ScriptVariable( value.as<std::string>() );
					break;
				}
				case sol::type::userdata:
				{
					const std::string& typeName = GetUserDataTypeName( value );
					Refl::MetaType type = Refl::ResolveMetaType( typeName.c_str() );
					if ( type.IsValid() )
					{
						a_Script.m_Variables[name] = ScriptVariable( type.FromVoid( value.pointer() ) );
					}
					break;
				}
				case sol::type::function:
				{
					a_Script.m_Functions[name] = value.as<ScriptFunction>();
					break;
				}
				default:
				{
					TE_CORE_WARN( "Unknown type in script environment" );
					break;
				}
			}
		}

		a_Script.m_CompileErrorMsg.clear();
		return true;
	}

	const std::string& ScriptEngine::GetUserDataTypeName( sol::userdata a_UserData )
	{
		if ( a_UserData.valid() && sol::type::userdata == a_UserData.get_type() )
		{
			auto metaTable = a_UserData[sol::metatable_key];
			for ( const std::string& typeName : Get().m_RegisteredTypes ) 
			{
				if ( metaTable[typeName] != sol::nil )
				{
					return typeName;
				}
			}
		}

		return "Unknown";
	}

}