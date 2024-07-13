#include "tripch.h"
#include "Asset.h"
#include <Tridium/Core/Application.h>

#ifdef IS_EDITOR
#include <Editor/Editor.h>
#endif // IS_EDITOR

#include "yaml-cpp/yaml.h"
#include <Tridium/IO/SerializationUtil.h>
#include <fstream>

namespace Tridium {

    template<typename T>
    bool AssetManager::HasAsset( const GUID& assetGUID )
    {
        if ( auto it = Get().m_Library.find( assetGUID ); it != Get().m_Library.end() )
        {
            if ( it->second->GetAssetType() != T::GetStaticType() )
            {
                return false;
            }

            return true;
        }

        return false;
    }

    template<typename T>
    Ref<T> AssetManager::GetAsset( const GUID& assetGUID, bool shouldLoad )
    {
        // Check if the asset is already loaded
        if ( auto it = Get().m_Library.find(assetGUID); it != Get().m_Library.end() )
        {
            if ( it->second->GetAssetType() != T::GetStaticType() )
            {
                TE_CORE_WARN( "Attempted to get asset of type '{0}' but the asset type is '{1}'", T::GetStaticType(), it->second->GetAssetType() );
                return nullptr;
            }

            return static_cast<Ref<T>>( it->second );
        }

        if ( !shouldLoad )
            return nullptr;

        Ref<T> asset = MakeRef<T>();
        asset->m_GUID = assetGUID;
        TODO( "Add asynchronous loading." );
        if ( !LoadAsset( assetGUID, asset ) )
            return false;

        return asset;
    }

    template<typename T>
    bool AssetManager::LoadAsset( const GUID& assetGUID, Ref<T>& outAsset )
    {
        if ( auto it = Get().m_Paths.find(assetGUID); it != Get().m_Paths.end() )
            return LoadAsset( path, outAsset );

        return false;
    }

    template<typename T>
    bool AssetManager::LoadAsset( const std::string& path, Ref<T>& outAsset )
    {
        if ( !T::Load( path, outAsset ) )
            return false;

        AddAsset( asset );
        return true;
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
            out << YAML::Key << "GUID" << YAML::Value << asset.first;
            out << YAML::Key << "Path" << YAML::Value << asset.second;
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;

        std::ofstream outFile( filepath );
        outFile << out.c_str();
    }

    bool AssetManager::Deserialize( const std::string& path )
    {
        YAML::Node data = YAML::LoadFile( path );
        YAML::Node assetsNode = data["Assets"];
        if ( !assetsNode )
            return false;

        auto& AssetPaths = m_Paths;

        for ( auto assetNode : assetsNode )
        {
            GUID guid;
            if ( auto guidNode = assetNode["GUID"] )
                guid = guidNode.as<GUID>();
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

            AssetPaths.emplace( guid, pathNode.as<std::string>() );
        }

        return true;
    }

    Ref<Asset> AssetManager::Import( const fs::path& path )
    {
        return Ref<Asset>();
    }

    void AssetManager::Init()
    {
        auto filepath = Application::GetActiveProject()->GetMetaDirectory() / ASSET_MANAGER_FILENAME;
        Deserialize( filepath.string() );
    }
}