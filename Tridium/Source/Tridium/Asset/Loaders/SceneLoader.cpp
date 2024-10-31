#include "tripch.h"
#include "SceneLoader.h"
#include <Tridium/IO/SceneSerializer.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/Core/Application.h>
#include <fstream>

namespace Tridium {

    void SceneLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
    {
		YAML::Emitter out;
		IO::SerializeToText( out, *( SharedPtrCast<Scene>( a_Asset ) ) );

        std::string path = AssetManager::Get<Editor::EditorAssetManager>()->GetAbsolutePath( a_MetaData.Path ).ToString();
		std::ofstream file( path.c_str() );
		file << out.c_str();
		file.close();
    }

    SharedPtr<Asset> SceneLoader::LoadAsset( const AssetMetaData& a_MetaData )
    {
        std::string path = AssetManager::Get<Editor::EditorAssetManager>()->GetAbsolutePath( a_MetaData.Path ).ToString();
		YAML::Node data;
        try
        {
            data = YAML::LoadFile( path );
        }
		catch ( const YAML::ParserException& e )
		{
			TE_CORE_ERROR( "Failed to parse scene file '{0}': {1}", path, e.what() );
			return nullptr;
		}

		SharedPtr<Scene> scene = MakeShared<Scene>();
		if ( !IO::DeserializeFromText( data, *scene ) )
		{
			TE_CORE_ERROR( "Failed to deserialize scene file '{0}'", path );
			return nullptr;
		}

		TODO( "Probably should not set the name here" );
		scene->SetName( a_MetaData.Name );

		return scene;
    }

}
