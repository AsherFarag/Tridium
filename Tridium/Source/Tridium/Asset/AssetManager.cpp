#include "tripch.h"
#include "AssetManager.h"
#include "Tridium/Core/Application.h"

#ifdef IS_EDITOR
#include <Editor/Editor.h>
#endif // IS_EDITOR

#include "yaml-cpp/yaml.h"
#include <Tridium/IO/SerializationUtil.h>
#include <fstream>

// Assets
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Material.h>

// Asset Meta Data
#include "Meta/AssetMetaData.h"
#include "Meta/ModelMetaData.h"
#include "Meta/ShaderMetaData.h"
#include "Meta/TextureMetaData.h"
#include "Meta/MaterialMetaData.h"

// Asset Loaders
#include "Loaders/ModelLoader.h"
#include "Loaders/ShaderLoader.h"
#include "Loaders/TextureLoader.h"
#include "Loaders/MaterialLoader.h"

namespace Tridium {

    SharedPtr<AssetManager> AssetManager::s_Instance = nullptr;

    void AssetManager::Serialize( const fs::path& path )
    {
        auto filepath = path / ASSET_MANAGER_FILENAME;
        YAML::Emitter out;

        out << YAML::Key << "Assets";
        out << YAML::BeginSeq;

        for ( auto& asset : Get()->m_Paths )
        {
            out << YAML::BeginMap;
            out << YAML::Key << "AssetHandle" << YAML::Value << asset.first;
            out << YAML::Key << "Path" << YAML::Value << asset.second.c_str();
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;

        std::ofstream outFile( filepath );
        outFile << out.c_str();
    }

    bool AssetManager::Deserialize( const fs::path& a_Path )
    {
        TODO( "This is jank" );
        std::ifstream file( a_Path );
        if ( !file )
            return false;

        YAML::Node data = YAML::LoadFile( a_Path.string() );
        YAML::Node assetsNode = data["Assets"];
        if ( !assetsNode )
            return false;

        auto& AssetPaths = m_Paths;

        for ( auto assetNode : assetsNode )
        {
            AssetHandle assetHandle;
            if ( auto assetHandleNode = assetNode["AssetHandle"] )
            {
                assetHandle = assetHandleNode.as<AssetHandle>();
            }
            else
            {
                TE_CORE_WARN( "Invalid Asset Node while reading meta file '{0}'", a_Path.string() );
                continue;
            }

            auto pathNode = assetNode["Path"];
            if ( !pathNode )
            {
                TE_CORE_WARN( "Invalid Asset Node while reading meta file '{0}'", a_Path.string() );
                continue;
            }

            AssetPaths.emplace( assetHandle, pathNode.as<std::string>() );
        }

        return true;
    }

    void AssetManager::Internal_AddAsset( const AssetRef<Asset>& a_Asset )
    {
        CHECK( a_Asset );
        Get()->m_Library[a_Asset->GetHandle()] = a_Asset;
    }

    AssetRef<Asset> AssetManager::Internal_LoadAsset( const fs::path& a_Path )
    {
        // Step 1. Open meta data file
        fs::path metaPath = a_Path;
        metaPath.append( ".meta" );
        UniquePtr<AssetMetaData> metaData;
        metaData.reset( AssetMetaData::Deserialize( metaPath ) );
        TE_CORE_ASSERT( metaData );

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
                TE_CORE_ERROR( "Asset meta data stored invalid AssetType. '{0}'", metaPath.string() );
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

    AssetRef<Asset> AssetManager::Internal_GetAsset( const AssetHandle& a_AssetHandle )
    {
        if ( auto it = m_Library.find( a_AssetHandle ); it != m_Library.end() )
        {
            return it->second;
        }

        return nullptr;
    }

    bool AssetManager::Internal_RemoveAsset( const AssetHandle& a_AssetHandle )
    {
        if ( auto it = m_Library.find( a_AssetHandle ); it != m_Library.end() )
        {
            m_Library.erase( it );
            return true;
        }

        return false;
    }

}