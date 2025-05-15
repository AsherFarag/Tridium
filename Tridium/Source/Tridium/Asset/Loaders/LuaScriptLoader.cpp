#include "tripch.h"
#include "LuaScriptLoader.h"
#include <Tridium/Scripting/Script.h>
#include <Tridium/Scripting/ScriptEngine.h>
#include <fstream>
#include <Tridium/Asset/EditorAssetManager.h>

namespace Tridium {

    void LuaScriptLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
    {
		std::string absolutePath = EditorAssetManager::Get()->GetAbsolutePath( a_MetaData.Path ).ToString();
		// Save the script to an existing text file or create a new one.
		std::ofstream out( a_MetaData.Path.ToString(), std::ios::out | std::ios::binary );
		if ( !out )
		{
			LOG( LogCategory::Script, Error, "Failed to save lua script to '{0}'", absolutePath );
			return;
		}

		// Save the script to the file.
		const ScriptAsset* script = Cast<const ScriptAsset*>( a_Asset.get() );
		out.write( script->GetSource().c_str(), script->GetSource().size() );
		out.close();
    }

    SharedPtr<Asset> LuaScriptLoader::LoadAsset( const AssetMetaData& a_MetaData )
    {
		std::string result;
		std::ifstream in( a_MetaData.Path.ToString(), std::ios::in | std::ios::binary);
		if ( !in )
		{
			LOG( LogCategory::Script, Error, "Could not open file '{0}'", a_MetaData.Path.ToString() );
			return nullptr;
		}

		std::string luaSource;
		in.seekg( 0, std::ios::end );
		luaSource.resize( in.tellg() );
		in.seekg( 0, std::ios::beg );
		in.read( &luaSource[0], luaSource.size() );
		in.close();

		SharedPtr<ScriptAsset> script = MakeShared<ScriptAsset>( std::move( luaSource ) );

		// Compile the script.
		ScriptEngine::Get()->RecompileScript( *script );

		return script;
    }

}
