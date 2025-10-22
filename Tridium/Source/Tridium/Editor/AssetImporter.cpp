#include "tripch.h"
#if IS_EDITOR
#include "AssetImporter.h"
#include <Tridium/oldAsset/EditorAssetManager.h>
#include <Tridium/Editor/Panels/Asset/MeshSourceImporterPanel.h>
#include <Tridium/Core/Application.h>

namespace Tridium {
    OldAssetHandle AssetImporter::ImportAsset( const FilePath& a_FilePath )
    {
        FilePath absolutePath = EditorAssetManager::Get()->GetAbsolutePath( a_FilePath );
		EAssetTypeOld assetType = GetAssetTypeFromFileExtension( absolutePath.GetExtension().ToString() );
        switch ( assetType )
        {
        case EAssetTypeOld::Scene:
			EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetTypeOld::Material:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetTypeOld::MeshSource:
        {
            Application::Get()->PushOverlay<MeshSourceImporterPanel>( absolutePath );
            break;
        }
        case EAssetTypeOld::StaticMesh:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetTypeOld::Shader:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetTypeOld::Texture:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetTypeOld::CubeMap:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        case EAssetTypeOld::LuaScript:
            EditorAssetManager::Get()->ImportAsset( absolutePath );
            break;
        }

		return OldAssetHandle::InvalidID;
    }
}

#endif // IS_EDITOR