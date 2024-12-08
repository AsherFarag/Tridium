#include "tripch.h"
#include "LuaScriptLoader.h"
#include <Tridium/Scripting/Script.h>
#include <Tridium/Scripting/ScriptEngine.h>
#include <fstream>

namespace Tridium {

    void LuaScriptLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
    {
    }

    SharedPtr<Asset> LuaScriptLoader::LoadAsset( const AssetMetaData& a_MetaData )
    {
		std::string result;
		std::ifstream in( a_MetaData.Path.ToString(), std::ios::in | std::ios::binary);
		if ( !in )
		{
			TE_CORE_ERROR( "Could not open file '{0}'", a_MetaData.Path.ToString() );
			return nullptr;
		}

		std::string luaSource;
		in.seekg( 0, std::ios::end );
		luaSource.resize( in.tellg() );
		in.seekg( 0, std::ios::beg );
		in.read( &luaSource[0], luaSource.size() );
		in.close();

		SharedPtr<Script::ScriptAsset> script = MakeShared<Script::ScriptAsset>( std::move( luaSource ) );

		// Compile the script.
		Script::ScriptEngine::RecompileScript( *script );

		return script;
    }

}
