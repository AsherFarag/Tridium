#include "tripch.h"
#include "AssetMetaData.h"
#include "ModelMetaData.h"
#include "ShaderMetaData.h"
#include "TextureMetaData.h"
#include "MaterialMetaData.h"
#include <Tridium/IO/SerializationUtil.h>

const char* ToString( const Tridium::EAssetType& rhs )
{
    switch ( rhs )
    {
        using enum Tridium::EAssetType;
    case Mesh:
        return "Mesh";
    case Shader:
        return "Shader";
    case Texture:
        return "Texture";
    case Material:
        return "Material";
    default:
        return "None";
    }
}

Tridium::EAssetType AssetTypeFromString( const std::string& string )
{
    using enum Tridium::EAssetType;
    static std::unordered_map<std::string, Tridium::EAssetType> s_AssetTypes =
    {
        {"Mesh", Mesh},
        {"Shader", Shader},
        {"Texture", Texture},
        {"Material", Material},
    };

    if ( auto it = s_AssetTypes.find( string ); it != s_AssetTypes.end() )
        return it->second;

    return None;
}

namespace YAML {

    template<>
    struct convert<Tridium::EAssetType>
    {
        static Node encode( const Tridium::EAssetType& rhs )
        {
            Node node;
            node.push_back( ToString(rhs) );
            return node;
        }

        static bool decode( const Node& node, Tridium::EAssetType& rhs )
        {
            rhs = AssetTypeFromString( node.as<std::string>() );
            return true;
        }
    };

}

namespace Tridium {

    YAML::Emitter& operator<<( YAML::Emitter& out, const EAssetType& v )
    {
        out << ToString( v );
        return out;
    }

    void AssetMetaData::Serialize( const fs::path& a_Path )
    {
        CHECK( a_Path.extension() != ".meta" );

        YAML::Emitter out;

        out << YAML::Key << "FileFormatVersion" << YAML::Value << FileFormatVersion;
        out << YAML::Key << "Handle" << YAML::Value << Handle;
        out << YAML::Key << "AssetType" << YAML::Value << AssetType;

        OnSerialize( out );
    }

    AssetMetaData* AssetMetaData::Deserialize( const fs::path& a_Path )
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile( a_Path.string() );
        }
        catch ( YAML::BadFile badFile )
        {
            TE_CORE_ERROR( "YAML: {0}", badFile.msg );
            return nullptr;
        }

        AssetMetaData tempMetaData;

        if ( auto node = data["FileFormatVersion"] ) { tempMetaData.FileFormatVersion = node.as<unsigned char>(); }
        else return nullptr;

        if ( auto node = data["Handle"] ) { tempMetaData.Handle = node.as<AssetHandle>(); }
        else return nullptr;

        if ( auto node = data["AssetType"] ) { tempMetaData.AssetType = AssetTypeFromString( node.as<std::string>() ); }
        else return nullptr;

        AssetMetaData* assetMetaData;

        switch ( tempMetaData.AssetType )
        {
            using enum Tridium::EAssetType;
            case Mesh:
                assetMetaData = new ModelMetaData{};
            case Shader:
                assetMetaData = new ShaderMetaData{};
            case Texture:
                assetMetaData = new TextureMetaData{};
            case Material:
                assetMetaData = new MaterialMetaData{};
            default:
                return nullptr;
        }

        assetMetaData->FileFormatVersion = tempMetaData.FileFormatVersion;
        assetMetaData->Handle = tempMetaData.Handle;
        assetMetaData->AssetType = tempMetaData.AssetType;

        return assetMetaData->OnDeserialize( data ) ? assetMetaData : nullptr;
    }

}