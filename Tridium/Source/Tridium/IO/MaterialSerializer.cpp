
#include "tripch.h"
#include "MaterialSerializer.h"
#include "Tridium/Asset/AssetManager.h"
#include "SerializationUtil.h"

#include <fstream>

namespace YAML {

	template<>
	struct convert<Tridium::EBlendMode>
	{
		static Node encode( const Tridium::EBlendMode& rhs )
		{
			Node node;
			switch ( rhs )
			{
			case Tridium::EBlendMode::Additive:
				node.push_back( "Additive" );
				break;
			case Tridium::EBlendMode::Subtractive:
				node.push_back( "Subtractive" );
				break;
			case Tridium::EBlendMode::Multiplicative:
				node.push_back( "Multiplicative" );
				break;
			case Tridium::EBlendMode::Alpha:
				node.push_back( "Alpha" );
				break;
			}
			return node;
		}

		static bool decode( const Node& node, Tridium::EBlendMode& rhs )
		{
			std::string val = node.as<std::string>();
			if ( val == "Additive" )
			{
				rhs = Tridium::EBlendMode::Additive;
				return true;
			}
			else if ( val == "Subtractive" )
			{
				rhs = Tridium::EBlendMode::Subtractive;
				return true;
			}
			else if ( val == "Multiplicative" )
			{
				rhs = Tridium::EBlendMode::Multiplicative;
				return true;
			}
			else if ( val == "Alpha" )
			{
				rhs = Tridium::EBlendMode::Alpha;
				return true;
			}

			return false;
		}
	};

}

namespace Tridium {

	YAML::Emitter& operator<<( YAML::Emitter& out, const EBlendMode& v )
	{
		switch ( v )
		{
		case EBlendMode::Additive:
			out << "Additive";
			break;
		case EBlendMode::Subtractive:
			out << "Subtractive";
			break;
		case EBlendMode::Multiplicative:
			out << "Multiplicative";
			break;
		case EBlendMode::Alpha:
			out << "Alpha";
			break;
		}

		return out;
	}

	MaterialSerializer::MaterialSerializer( const AssetRef<Material>& a_Material )
		: m_Material ( a_Material )
	{
	}

	void MaterialSerializer::SerializeText( const std::string& filepath )
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Shader" << YAML::Value << m_Material->m_Shader.GetAssetHandle();
		out << YAML::Key << "Parent Material" << YAML::Value << m_Material->m_Parent.GetAssetHandle();
		out << YAML::Key << "Blend Mode"   << YAML::Value << m_Material->BlendMode;

		out << YAML::Key << "Properties" << YAML::Value << YAML::BeginSeq;

		for ( auto&[name, prop] : m_Material->m_Properties )
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << name;
			out << YAML::Key << "Type" << YAML::Value << (int)prop.Type;
			out << YAML::Key << "Value";

		#define PRINT_ARRAY(type)\
			out << YAML::Value << YAML::BeginSeq;\
			std::vector<type>& arr = std::get<std::vector<type>>( prop.Value );\
			for( type& it : arr )\
			{\
				out << it;\
			}\
			out << YAML::EndSeq;

			switch ( prop.Type )
			{
			case EPropertyType::Int:
			{
				out << YAML::Value << std::get<int>( prop.Value );
				break;
			}
			case EPropertyType::IntArray:
			{
				PRINT_ARRAY( int )
					break;
			}
			case EPropertyType::Float:
			{
				out << YAML::Value << std::get<float>( prop.Value );
				break;
			}
			case EPropertyType::FloatArray:
			{
				PRINT_ARRAY( float )
				break;
			}
			case EPropertyType::Color:
			{
				out << YAML::Value << std::get<Color>( prop.Value );
				break;
			}
			case EPropertyType::ColorArray:
			{
				out << YAML::Value << YAML::BeginSeq; std::vector<Color>& arr = std::get<std::vector<Color>>( prop.Value ); for ( auto& it : arr ) {
					out << it;
				} out << YAML::EndSeq;
					break;
			}
			case EPropertyType::Vector4:
			{
				out << YAML::Value << std::get<Vector4>( prop.Value );
				break;
			}
			case EPropertyType::Vector4Array:
			{
				PRINT_ARRAY( Vector4 )
				break;
			}
			case EPropertyType::Matrix4:
			{
				out << YAML::Value << std::get<Matrix4>( prop.Value );
				break;
			}
			case EPropertyType::Matrix4Array:
			{
				PRINT_ARRAY( Matrix4 )
				break;
			}
			case EPropertyType::Texture:
			{
				out << YAML::Value << std::get<AssetRef<Texture>>( prop.Value ).GetAssetHandle();
				break;
			}
			default:
				break;
			}

		#undef	PRINT_ARRAY

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::EndMap;

		std::ofstream outFile( filepath );
		outFile << out.c_str();
	}

	bool MaterialSerializer::DeserializeText( const std::string& a_Path )
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile( a_Path );
		}
		catch ( YAML::BadFile badFile )
		{
			TE_CORE_ERROR( "YAML: {0}", badFile.msg );
			return false;
		}

		m_Material->m_Path = a_Path;

		m_Material->m_Shader = AssetManager::LoadAsset<Shader>( data["Shader"].as<AssetHandle>() );
		m_Material->m_Parent = AssetManager::LoadAsset<Material>( data["Parent Material"].as<AssetHandle>() );
		m_Material->BlendMode = data["Blend Mode"].as<EBlendMode>();

		for ( auto propNode : data["Properties"] )
		{
			auto nameNode = propNode["Name"];
			auto typeNode = propNode["Type"];
			auto valueNode = propNode["Value"];

			if ( !nameNode || !typeNode || !valueNode )
				continue;

			EPropertyType type = static_cast<EPropertyType>( typeNode.as<int>() );
			std::string name = nameNode.as<std::string>();
			m_Material->m_Properties.insert( { name, { type } } );

			switch ( type )
			{
			case EPropertyType::Int:
				m_Material->m_Properties[name].Value = valueNode.as<int>();
				break;
			case EPropertyType::IntArray:
				m_Material->m_Properties[name].Value = valueNode.as<std::vector<int>>();
				break;
			case EPropertyType::Float:
				m_Material->m_Properties[name].Value = valueNode.as<float>();
				break;
			case EPropertyType::FloatArray:
				m_Material->m_Properties[name].Value = valueNode.as<std::vector<float>>();
				break;
			case EPropertyType::Color:
				m_Material->m_Properties[name].Value = valueNode.as<Color>();
				break;
			case EPropertyType::ColorArray:
				m_Material->m_Properties[name].Value = valueNode.as<std::vector<Color>>();
				break;
			case EPropertyType::Vector4:
				m_Material->m_Properties[name].Value = valueNode.as<Vector4>();
				break;
			case EPropertyType::Vector4Array:
				m_Material->m_Properties[name].Value = valueNode.as<std::vector<Vector4>>();
				break;
			case EPropertyType::Matrix4:
				m_Material->m_Properties[name].Value = valueNode.as<Matrix4>();
				break;
			case EPropertyType::Matrix4Array:
				m_Material->m_Properties[name].Value = valueNode.as<std::vector<Matrix4>>();
				break;
			case EPropertyType::Texture:
				m_Material->m_Properties[name].Value = AssetRef<Texture>{ valueNode.as<AssetHandle>() };
				break;
			default:
				break;
			}
		}

		return true;
	}

}