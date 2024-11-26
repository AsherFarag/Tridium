#include "tripch.h"
#if IS_EDITOR
#include "AssetImporter.h"
#include <Tridium/Asset/EditorAssetManager.h>
#include <Editor/Panels/Asset/MeshSourceImporterPanel.h>

namespace Tridium::Editor {
    AssetHandle AssetImporter::ImportAsset( const IO::FilePath& a_FilePath )
    {
        IO::FilePath absolutePath = EditorAssetManager::Get()->GetAbsolutePath( a_FilePath );
		EAssetType assetType = GetAssetTypeFromFileExtension( absolutePath.GetExtension() );
        switch ( assetType )
        {
        case EAssetType::Scene:
			EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetType::Material:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetType::MeshSource:
        {
            Application::Get().PushOverlay( new Editor::MeshSourceImporterPanel( absolutePath ) );
            break;
        }
        case EAssetType::StaticMesh:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetType::Shader:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetType::Texture:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetType::CubeMap:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetType::Lua:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        }

		return AssetHandle::InvalidGUID;
    }
}

#endif // IS_EDITOR