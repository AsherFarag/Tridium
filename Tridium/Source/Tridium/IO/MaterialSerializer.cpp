
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
		TE_CORE_TRACE( "Begin Serializing Material" );
		TE_CORE_TRACE( "Serializing to \"{0}\"", filepath );

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Material";
		out << YAML::Value << m_Material->GetHandle();

		out << YAML::Key << "Shader"; out << YAML::Value << ( m_Material->GetShader() ? m_Material->GetShader()->GetHandle() : AssetHandle{} );

		out << YAML::Key << "Parent Material" << YAML::Value << ( m_Material->m_Parent ? m_Material->m_Parent->GetHandle() : AssetHandle{} );

		out << YAML::Key << "Blend Mode"   << YAML::Value << m_Material->BlendMode;
		out << YAML::Key << "Color"        << YAML::Value << m_Material->Color;
		out << YAML::Key << "Reflectivity" << YAML::Value << m_Material->Reflectivity;
		out << YAML::Key << "Refraction"   << YAML::Value << m_Material->Refraction;

		out << YAML::Key << "BaseColorTexture" << YAML::Value << ( m_Material->BaseColorTexture ? m_Material->BaseColorTexture->GetHandle() : AssetHandle{} );
		out << YAML::Key << "NormalMapTexture" << YAML::Value << ( m_Material->NormalMapTexture ? m_Material->NormalMapTexture->GetHandle() : AssetHandle{} );
		out << YAML::Key << "MetallicTexture"  << YAML::Value << ( m_Material->MetallicTexture ? m_Material->MetallicTexture->GetHandle()   : AssetHandle{} );
		out << YAML::Key << "RoughnessTexture" << YAML::Value << ( m_Material->RoughnessTexture ? m_Material->RoughnessTexture->GetHandle() : AssetHandle{} );
		out << YAML::Key << "EmissiveTexture"  << YAML::Value << ( m_Material->EmissiveTexture ? m_Material->EmissiveTexture->GetHandle()   : AssetHandle{} );

		out << YAML::EndMap;

		std::ofstream outFile( filepath );
		outFile << out.c_str();

		TE_CORE_TRACE( "End Serializing Material" );
	}

	bool MaterialSerializer::DeserializeText( const std::string& filepath )
	{
		TODO( "This is jank" );
		std::ifstream file( filepath );
		if ( !file )
			return false;

		TE_CORE_TRACE( "Begin Deserializing Material from '{0}'", filepath );

		YAML::Node data = YAML::LoadFile( filepath );
		if ( !data["Material"] )
			return false;

		m_Material->m_Path = filepath;

		m_Material->m_Handle = data["Material"].as<AssetHandle>();
		m_Material->m_Shader = AssetManager::GetAsset<Shader>( data["Shader"].as<std::string>() );
		m_Material->m_Parent = AssetManager::GetAsset<Material>( data["Parent Material"].as<AssetHandle>() );

		m_Material->BlendMode = data["Blend Mode"].as<EBlendMode>();
		m_Material->Color = data["Color"].as<Color>();
		m_Material->Reflectivity = data["Reflectivity"].as<float>();
		m_Material->Refraction = data["Refraction"].as<float>();
									  
		m_Material->BaseColorTexture = AssetManager::GetAsset<Texture>( data["BaseColorTexture"].as<std::string>() );
		m_Material->NormalMapTexture = AssetManager::GetAsset<Texture>( data["NormalMapTexture"].as<std::string>() );
		m_Material->MetallicTexture  = AssetManager::GetAsset<Texture>( data["MetallicTexture"].as<std::string>() );
		m_Material->RoughnessTexture = AssetManager::GetAsset<Texture>( data["RoughnessTexture"].as<std::string>() );
		m_Material->EmissiveTexture  = AssetManager::GetAsset<Texture>( data["EmissiveTexture"].as<std::string>() );

		TE_CORE_TRACE( "End Deserializing Material" );

		return true;
	}

}