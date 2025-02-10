#include "tripch.h"
#include "ScriptEngine.h"

#include "Tridium/Asset/AssetManager.h"
#include <Tridium/ECS/Components/Types.h>

#include <any>

// TEMP!
#include <Tridium/Asset/EditorAssetManager.h>
#include <fstream>

namespace Tridium {

	bool ScriptEngine::Init()
	{
		using RegisterScriptableFunc = Refl::Props::RegisterScriptableProp::Type;

		// Open the standard libraries
		m_LuaState.open_libraries( sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::os, sol::lib::io );

		// Register reflected types into the Lua state
		for ( const auto&& [id, type] : Refl::ResolveMetaTypes() )
		{
			Refl::MetaType metaType = type;
			if ( !metaType.IsValid() || !HasFlag( metaType.GetClassFlags(), Refl::EClassFlags::Scriptable ) )
				continue;

			Refl::MetaAttribute regFuncAtt = metaType.GetMetaAttribute( Refl::Props::RegisterScriptableProp::ID );
			if ( !regFuncAtt || !regFuncAtt.value().allow_cast<RegisterScriptableFunc>( ) )
				continue;

			RegisterScriptableFunc regFunc = regFuncAtt.value().cast<RegisterScriptableFunc>();
			regFunc( *this );
		}

		return true;
	}

	void ScriptEngine::Shutdown()
	{
	}

	bool ScriptEngine::RecompileScript( ScriptAsset& a_Script )
	{
		// Clear the current environment
		a_Script.m_Environment = {};
		a_Script.m_Variables.clear();
		a_Script.m_Functions.clear();

		a_Script.m_LoadResult = m_LuaState.load( a_Script.m_Source );
		if ( !a_Script.m_LoadResult.valid() )
		{
			sol::error err = a_Script.m_LoadResult;
			a_Script.m_CompileErrorMsg = err.what();
			TE_CORE_ERROR( "SCRIPT COMPILATION ERROR: {0}", a_Script.m_CompileErrorMsg );
			return false;
		}

		a_Script.m_Environment = sol::environment( m_LuaState, sol::create, m_LuaState.globals() );
		sol::protected_function scriptFunc = a_Script.m_LoadResult;
		a_Script.m_Environment.set_on( scriptFunc );

		// We need to execute the script in order to extract the variables and functions from the environment.
		TODO( "Not sure if this is the best way to do this and why I have to do this. Asher research this later idiot" );
		scriptFunc();

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
					const StringView typeName = GetUserDataTypeName( value );
					if ( Refl::MetaType type = Refl::ResolveMetaType( typeName.data() ) )
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

	void ScriptEngine::RecompileAllScripts()
	{
		TODO( "TEMP EDITOR ONLY HERE" );
		EditorAssetManager* assetManager = EditorAssetManager::Get();
		for ( SharedPtr<ScriptAsset> script : AssetManager::GetAssetsOfType<ScriptAsset>() )
		{
			const AssetMetaData& assetData = assetManager->GetAssetMetaData( script->GetHandle() );
			if ( assetData.IsValid() )
			{
				std::string path = assetManager->GetAbsolutePath( assetData.Path ).ToString();
				std::ifstream file( path );
				if ( file.is_open() )
				{
					std::string source( ( std::istreambuf_iterator<char>( file ) ), std::istreambuf_iterator<char>() );
					script->m_Source = std::move( source );
					RecompileScript( *script );
					TE_CORE_INFO( "Recompiled script: {0}", assetData.Name );
				}
			}
		}
	}

	const StringView ScriptEngine::GetUserDataTypeName( sol::userdata a_UserData )
	{
		if ( a_UserData.valid() && sol::type::userdata == a_UserData.get_type() )
		{
			auto metaTable = a_UserData[sol::metatable_key];
			for ( const String& typeName : m_RegisteredTypes ) 
			{
				if ( metaTable[typeName] != sol::nil )
				{
					return typeName;
				}
			}
		}

		static constexpr StringView s_Unknown = "Unknown";
		return s_Unknown;
	}

}