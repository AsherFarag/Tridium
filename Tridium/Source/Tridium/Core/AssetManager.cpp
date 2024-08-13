#include "tripch.h"
#include "AssetManager.h"
#include "Tridium/Core/Application.h"

#ifdef IS_EDITOR
#include <Editor/Editor.h>
#endif // IS_EDITOR

#include "yaml-cpp/yaml.h"
#include <Tridium/IO/SerializationUtil.h>
#include <fstream>

namespace Tridium {

    void AssetManager::ReleaseAsset( const AssetHandle& a_AssetHandle )
    {
        Get()->ReleaseAsset( a_AssetHandle );
    }

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
                assetHandle = assetHandleNode.as<AssetHandle>();
            else
            {
                TE_CORE_WARN( "Invalid Asset Node while reading meta file '{0}'", a_Path );
                continue;
            }

            auto pathNode = assetNode["Path"];
            if ( !pathNode )
            {
                TE_CORE_WARN( "Invalid Asset Node while reading meta file '{0}'", a_Path );
                continue;
            }

            AssetPaths.emplace( assetHandle, pathNode.as<std::string>() );
        }

        return true;
    }

    void AssetManager::Internal_AddAsset( const AssetRef<Asset>& a_Asset )
    {
        TE_CORE_ASSERT( a_Asset );
        Get()->m_Library[a_Asset->GetHandle()] = a_Asset;
    }

    AssetRef<Asset> AssetManager::Internal_LoadAsset( const AssetHandle& a_AssetHandle )
    {
        return AssetRef<Asset>();
    }

    AssetRef<Asset> AssetManager::Internal_LoadAsset( const fs::path& a_Path )
    {
        return AssetRef<Asset>();
    }

    AssetRef<Asset> AssetManager::Internal_GetAsset( const AssetHandle& a_AssetHandle )
    {
        if ( auto it = m_Library.find( a_AssetHandle ); it != m_Library.end() )
        {
            return it->second;
        }

        return nullptr;
    }

    const fs::path& AssetManager::Internal_GetPath( const AssetHandle& a_AssetHandle )
    {
        return {};
    }

    const AssetHandle& AssetManager::Internal_GetAssetHandle( const fs::path& a_Path )
    {
        fs::path path = a_Path;
        if ( !path.has_extension() || path.extension() != META_FILE_EXTENSION )
        {
            path.append( META_FILE_EXTENSION );
        }

        return {};
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