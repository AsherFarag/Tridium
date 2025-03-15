#include "tripch.h"
#if IS_EDITOR
#include "AssetImporter.h"
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/Editor/Panels/Asset/MeshSourceImporterPanel.h>
#include <Tridium/Core/Application.h>

namespace Tridium {
    AssetHandle AssetImporter::ImportAsset( const FilePath& a_FilePath )
    {
        FilePath absolutePath = EditorAssetManager::Get()->GetAbsolutePath( a_FilePath );
		EAssetType assetType = GetAssetTypeFromFileExtension( absolutePath.GetExtension().ToString() );
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
            Application::Get()->PushOverlay( new MeshSourceImporterPanel( absolutePath ) );
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
        case EAssetType::LuaScript:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        }

		return AssetHandle::InvalidID;
    }
}

#endif // IS_EDITOR