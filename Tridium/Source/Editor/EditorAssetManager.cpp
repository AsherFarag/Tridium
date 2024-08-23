#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorAssetManager.h"

#include <Tridium/Asset/AssetFileExtensions.h>
#include "Editor.h"
#include "Panels/MeshImporterPanel.h"

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
#include <Tridium/Asset/Meta/SceneMetaData.h>

// Asset Loaders
#include <Tridium/Asset/Loaders/ModelLoader.h>
#include <Tridium/Asset/Loaders/ShaderLoader.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>
#include <Tridium/Asset/Loaders/MaterialLoader.h>

namespace Tridium::Editor {

    namespace Util {
    }

#pragma region Import

        void EditorAssetManager::ImportFBX( const IO::FilePath& a_Path )
        {

        }

        void EditorAssetManager::ImportMesh( const IO::FilePath& a_Path )
        {
            GetEditorLayer()->PushPanel<MeshImporterPanel>( a_Path );
        }

        void EditorAssetManager::ImportTexture( const IO::FilePath& a_Path )
        {
            TextureMetaData metadata;
            if ( Texture* texture = Texture::LoaderType::Load( a_Path, metadata ) )
            {
                Internal_AddAsset( texture );
                metadata.Serialize( a_Path );
            }
        }

        void EditorAssetManager::ImportShader( const IO::FilePath& a_Path )
        {
            ShaderMetaData metadata;
            if ( Shader* shader = Shader::LoaderType::Load( a_Path, metadata ) )
            {
                Internal_AddAsset( shader );
                metadata.Serialize( a_Path );
            }
        }

        void EditorAssetManager::ImportMaterial( const IO::FilePath& a_Path )
        {
            MaterialMetaData metadata;
            if ( Material* material = Material::LoaderType::Load( a_Path, metadata ) )
            {
                Internal_AddAsset( material );
                metadata.Serialize( a_Path );
            }
        }
        
        void EditorAssetManager::ImportScene( const IO::FilePath& a_Path )
        {
            SceneMetaData metadata;
        }

#pragma endregion

    void EditorAssetManager::Internal_ImportAsset( const IO::FilePath a_Path, bool a_Override )
    {
        // If we do not want to reimport an asset and there is an existing meta data file, return
        if ( !a_Override && AssetMetaData::Deserialize( a_Path.ToString().append( IO::MetaExtension ) ) )
            return;

        std::string fileExt = a_Path.GetExtension().ToString();

        if ( fileExt == IO::FBXExtension )
        {
            ImportFBX( a_Path );
            return;
        }

        EAssetType assetType = IO::GetAssetTypeFromExtension( fileExt );

        switch ( assetType )
        {
            using enum EAssetType;
        case None:
            return;
        case Mesh:
            ImportMesh( a_Path );
            break;
        case Shader:
            ImportShader( a_Path );
            break;
        case Texture:
            ImportTexture( a_Path );
            break;
        case Material:
            ImportMaterial( a_Path );
            break;
        case Folder:
            break;
        case Lua:
            break;
        case Project:
            break;
        case Scene:
            ImportScene( a_Path );
            break;
        }
    }

    AssetRef<Asset> EditorAssetManager::Internal_LoadAsset( const IO::FilePath& a_Path )
    {
        CHECK( a_Path.GetExtension().ToString() != IO::MetaExtension );

        // Step 1. Try to find the corresponding meta data file
        IO::FilePath metaPath = a_Path;
        metaPath += IO::MetaExtension;
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