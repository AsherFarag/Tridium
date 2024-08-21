#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorAssetManager.h"

#include <Tridium/Asset/AssetFileExtensions.h>

// Assets
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Material.h>

// Asset Meta Data
#include <Tridium/Asset/Meta/AssetMetaData.h>
#include <Tridium/Asset/Meta/ModelMetaData.h>
#include <Tridium/Asset/Meta/ShaderMetaData.h>
#include <Tridium/Asset/Meta/TextureMetaData.h>
#include <Tridium/Asset/Meta/MaterialMetaData.h>

// Asset Loaders
#include <Tridium/Asset/Loaders/ModelLoader.h>
#include <Tridium/Asset/Loaders/ShaderLoader.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>
#include <Tridium/Asset/Loaders/MaterialLoader.h>

namespace Tridium::Editor {

    void EditorAssetManager::Internal_ImportAsset( const IO::FilePath a_Path )
    {
        std::string fileExt = a_Path.GetExtension().ToString();
        if ( fileExt == IO::FBXExtension )
        {
            Internal_ImportFBX( a_Path );
            return;
        }

        EAssetType assetType = IO::GetAssetTypeFromExtension( fileExt );
        if ( assetType == EAssetType::None )
            return;
    }

    void EditorAssetManager::Internal_ImportFBX( const IO::FilePath& a_Path )
    {
    }

    AssetRef<Asset> EditorAssetManager::Internal_LoadAsset( const IO::FilePath& a_Path )
    {
        CHECK( a_Path.GetExtension().ToString() != IO::MetaExtension );

        // Step 1. Try to find the corresponding meta data file
        IO::FilePath metaPath = a_Path;
        metaPath.Append( IO::MetaExtension );
        UniquePtr<AssetMetaData> metaData;
        metaData.reset( AssetMetaData::Deserialize( metaPath ) );

        // If the meta data file doesn't exist, we need to import the asset and create the meta file
        if ( !metaData )
        {
            TE_CORE_ERROR( "Attempted to load asset with missing .meta file. '{0}'", a_Path.ToString() );
            return nullptr;
        }

        // Step 2. Check if this asset it already loaded
        if ( auto asset = Internal_GetAsset( metaData->Handle ) )
            return asset;

        // Step 3. Load the asset with it's matching loader
        AssetRef<Asset> asset;
        switch ( metaData->AssetType )
        {
            case EAssetType::Mesh:
            {
                asset = Mesh::LoaderType::Load( a_Path, *static_cast<ModelMetaData*>( metaData.get() ) );
                break;
            }
            case EAssetType::Shader:
            {
                asset = Shader::LoaderType::Load( a_Path, *static_cast<ShaderMetaData*>( metaData.get() ) );
                break;
            }
            case EAssetType::Texture:
            {
                asset = Texture::LoaderType::Load( a_Path, *static_cast<TextureMetaData*>( metaData.get() ) );
                break;
            }
            case EAssetType::Material:
            {
                asset = Material::LoaderType::Load( a_Path, *static_cast<MaterialMetaData*>( metaData.get() ) );
                break;
            }
            default:
            {
                TE_CORE_ERROR( "Asset meta data stored invalid AssetType. '{0}'", metaPath.ToString() );
                return nullptr;
            }
        }

        // Step 4. If the asset was successfuly loaded, add it to m_Library
        // Else, return nullptr

        if ( asset == nullptr )
            return nullptr;

        Internal_AddAsset( asset );

        return asset;
    }

}

#endif