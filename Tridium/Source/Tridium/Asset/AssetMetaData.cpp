#include "tripch.h"
#include <Tridium/Utils/Reflection/Reflection.h>
#include "AssetMetaData.h"
#include <Tridium/Asset/AssetFileExtensions.h>
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

    void AssetMetaData::Serialize( const IO::FilePath& a_Path )
    {
        //CHECK( a_Path.GetExtension().ToString() != IO::MetaExtension );

        //YAML::Emitter out;

        //out << YAML::BeginMap;
        //out << YAML::Key << "FileFormatVersion" << YAML::Value << FileFormatVersion;
        //out << YAML::Key << "Handle" << YAML::Value << Handle;
        //out << YAML::Key << "AssetType" << YAML::Value << AssetType;

        //OnSerialize( out );

        //out << YAML::EndMap;

        //std::string metaPath = a_Path.ToString();
        //metaPath.append( IO::MetaExtension );
        //std::ofstream outFile( metaPath );
        //outFile << out.c_str();
    }

    AssetMetaData* AssetMetaData::Deserialize( const IO::FilePath& a_Path )
    {
        return nullptr;
        //YAML::Node data;
        //try
        //{
        //    data = YAML::LoadFile( a_Path.ToString() );
        //}
        //catch ( YAML::BadFile badFile )
        //{
        //    TE_CORE_ERROR( "YAML: {0}", badFile.msg );
        //    return nullptr;
        //}

        //AssetMetaData tempMetaData;

        //if ( auto node = data["FileFormatVersion"] ) { tempMetaData.FileFormatVersion = node.as<uint32_t>(); }
        //else return nullptr;

        //if ( auto node = data["Handle"] ) { tempMetaData.Handle = node.as<AssetHandle>(); }
        //else return nullptr;

        //if ( auto node = data["AssetType"] ) { tempMetaData.AssetType = AssetTypeFromString( node.as<std::string>() ); }
        //else return nullptr;

        //AssetMetaData* assetMetaData;

        //switch ( tempMetaData.AssetType )
        //{
        //    using enum Tridium::EAssetType;
        //    case Mesh:
        //        assetMetaData = new ModelMetaData{};
        //        break;
        //    case Shader:
        //        assetMetaData = new ShaderMetaData{};
        //        break;
        //    case Texture:
        //        assetMetaData = new TextureMetaData{};
        //        break;
        //    case Material:
        //        assetMetaData = new MaterialMetaData{};
        //        break;
        //    default:
        //        return nullptr;
        //}

        //assetMetaData->FileFormatVersion = tempMetaData.FileFormatVersion;
        //assetMetaData->Handle = tempMetaData.Handle;
        //assetMetaData->AssetType = tempMetaData.AssetType;

        //return assetMetaData->OnDeserialize( data ) ? assetMetaData : nullptr;
    }

    ModelMetaData::ModelMetaData()
        : AssetMetaData( EAssetType::Mesh )
    {
    }

    void ModelMetaData::OnSerialize( YAML::Emitter& a_Out )
    {
        //a_Out << YAML::Key << "Import Settings" << YAML::BeginMap;

        //a_Out << YAML::Key << "PostProcessFlags" << YAML::Value << ImportSettings.PostProcessFlags;
        //a_Out << YAML::Key << "Scale" << YAML::Value << ImportSettings.Scale;
        //a_Out << YAML::Key << "DiscardLocalData" << YAML::Value << ImportSettings.DiscardLocalData;

        //a_Out << YAML::EndMap;
    }

    bool ModelMetaData::OnDeserialize( YAML::Node& a_Data )
    {
        return false;
        //auto importSettingsNode = a_Data["Import Settings"];
        //if ( !importSettingsNode )
        //    return true;

        //if ( auto node = importSettingsNode["PostProcessFlags"] )
        //    ImportSettings.PostProcessFlags = node.as<unsigned int>();

        //if ( auto node = importSettingsNode["Scale"] )
        //    ImportSettings.Scale = node.as<float>();

        //if ( auto node = importSettingsNode["DiscardLocalData"] )
        //    ImportSettings.Scale = node.as<bool>();

        //return true;
    }

    BEGIN_REFLECT( AssetMetaData );
        PROPERTY( FileFormatVersion );
        PROPERTY( Handle );
        PROPERTY( AssetType );
    END_REFLECT( AssetMetaData );

    BEGIN_REFLECT( ShaderMetaData );
        BASE( AssetMetaData );
    END_REFLECT( ShaderMetaData );

    BEGIN_REFLECT( SceneMetaData );
        BASE( AssetMetaData );
    END_REFLECT( SceneMetaData );

    BEGIN_REFLECT( TextureMetaData );
        BASE( AssetMetaData );
	END_REFLECT( TextureMetaData );

	BEGIN_REFLECT( MaterialMetaData )
		BASE( AssetMetaData )
	END_REFLECT( MaterialMetaData );

    BEGIN_REFLECT( ModelMetaData )
		BASE( AssetMetaData )
        PROPERTY( ImportSettings )
	END_REFLECT( ModelMetaData );
}