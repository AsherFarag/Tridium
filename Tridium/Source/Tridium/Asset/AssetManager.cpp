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

// Asset Loaders
#include "Loaders/ModelLoader.h"
#include "Loaders/ShaderLoader.h"
#include "Loaders/TextureLoader.h"
#include "Loaders/MaterialLoader.h"

namespace Tridium {

    SharedPtr<AssetManager> AssetManager::s_Instance = nullptr;

    void AssetManager::Serialize( const IO::FilePath& a_Path )
    {
        auto filepath = a_Path / ASSET_MANAGER_FILENAME;
        YAML::Emitter out;

        out << YAML::Key << "Assets";
        out << YAML::BeginSeq;

        for ( auto& asset : Get()->m_Paths )
        {
            out << YAML::BeginMap;
            out << YAML::Key << "AssetHandle" << YAML::Value << asset.first;
            out << YAML::Key << "Path" << YAML::Value << asset.second.ToString();
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;

        std::ofstream outFile( filepath.ToString() );
        outFile << out.c_str();
    }

    bool AssetManager::Deserialize( const IO::FilePath& a_Path )
    {
        TODO( "This is jank" );
        std::ifstream file( a_Path.ToString() );
        if ( !file )
            return false;

        YAML::Node data = YAML::LoadFile( a_Path.ToString() );
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
                TE_CORE_WARN( "Invalid Asset Node while reading meta file '{0}'", a_Path.ToString() );
                continue;
            }

            auto pathNode = assetNode["Path"];
            if ( !pathNode )
            {
                TE_CORE_WARN( "Invalid Asset Node while reading meta file '{0}'", a_Path.ToString() );
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