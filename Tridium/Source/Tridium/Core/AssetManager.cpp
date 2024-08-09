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
        Get().ReleaseAsset( a_AssetHandle );
    }

    void AssetManager::Serialize( const fs::path& path )
    {
        auto filepath = path / ASSET_MANAGER_FILENAME;
        YAML::Emitter out;

        out << YAML::Key << "Assets";
        out << YAML::BeginSeq;

        for ( auto& asset : Get().m_Paths )
        {
            out << YAML::BeginMap;
            out << YAML::Key << "AssetHandle" << YAML::Value << asset.first;
            out << YAML::Key << "Path" << YAML::Value << asset.second;
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;

        std::ofstream outFile( filepath );
        outFile << out.c_str();
    }

    bool AssetManager::Deserialize( const std::string& path )
    {
        TODO( "This is jank" );
        std::ifstream file( path );
        if ( !file )
            return false;

        YAML::Node data = YAML::LoadFile( path );
        YAML::Node assetsNode = data["Assets"];
        if ( !assetsNode )
            return false;

        auto& AssetPaths = m_Paths;

        for ( auto assetNode : assetsNode )
        {
            AssetHandle AssetHandle;
            if ( auto AssetHandleNode = assetNode["AssetHandle"] )
                AssetHandle = AssetHandleNode.as<AssetHandle>();
            else
            {
                TE_CORE_WARN( "Invalid Asset Node while reading meta file '{0}'", path );
                continue;
            }

            auto pathNode = assetNode["Path"];
            if ( !pathNode )
            {
                TE_CORE_WARN( "Invalid Asset Node while reading meta file '{0}'", path );
                continue;
            }

            AssetPaths.emplace( AssetHandle, pathNode.as<std::string>() );
        }

        return true;
    }

    void AssetManager::Init()
    {
        auto filepath = Application::GetActiveProject()->GetMetaDirectory() / ASSET_MANAGER_FILENAME;
        Deserialize( filepath.string() );
    }

    void AssetManager::Internal_AddAsset( const AssetRef<Asset>& asset )
    {
        Get().m_Library[asset->GetHandle()] = asset;
    }

    AssetRef<Asset> AssetManager::Internal_GetAsset( const AssetHandle& a_AssetHandle )
    {
        if ( auto it = m_Library.find( a_AssetHandle ); it != m_Library.end() )
        {
            return it->second;
        }

        return nullptr;
    }

    const std::string& AssetManager::Internal_GetPath( const AssetHandle& a_AssetHandle )
    {
        // TODO: insert return statement here
    }

    const AssetHandle& AssetManager::Internal_GetAssetHandle( const std::string& a_Path )
    {
        // TODO: insert return statement here
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