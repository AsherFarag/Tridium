#include "tripch.h"
#include "MaterialSerializer.h"
#include <Tridium/Rendering/Material.h>
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

	MaterialSerializer::MaterialSerializer( const Ref<Material>& material )
		: m_Material ( material )
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

		Ref<Shader> shader = ShaderLibrary::GetShader( m_Material->GetShader() );
		out << YAML::Key << "Shader"; out << YAML::Value << ( shader ? shader->GetPath() : "" );

		out << YAML::Key << "Parent Material";
		out << YAML::Value << m_Material->m_Parent;

		out << YAML::Key << "Blend Mode"; out << YAML::Value << m_Material->BlendMode;
		out << YAML::Key << "Color"; out << YAML::Value << m_Material->Color;
		out << YAML::Key << "Reflectivity"; out << YAML::Value << m_Material->Reflectivity;
		out << YAML::Key << "Refraction"; out << YAML::Value << m_Material->Refraction;

		Ref<Texture> tex = nullptr;
		out << YAML::Key << "BaseColorTexture"; 
		tex = TextureLibrary::GetTexture( m_Material->BaseColorTexture );
		out << YAML::Value << ( tex ? tex->GetPath() : "" );
		tex = TextureLibrary::GetTexture( m_Material->NormalMapTexture );
		out << YAML::Key << "NormalMapTexture";
		out << YAML::Value << ( tex ? tex->GetPath() : "" );
		tex = TextureLibrary::GetTexture( m_Material->MetallicTexture );
		out << YAML::Key << "MetallicTexture";
		out << YAML::Value << ( tex ? tex->GetPath() : "" );
		tex = TextureLibrary::GetTexture( m_Material->RoughnessTexture );
		out << YAML::Key << "RoughnessTexture";
		out << YAML::Value << ( tex ? tex->GetPath() : "" );
		tex = TextureLibrary::GetTexture( m_Material->EmissiveTexture );
		out << YAML::Key << "EmissiveTexture";
		out << YAML::Value << ( tex ? tex->GetPath() : "" );

		out << YAML::Key << "Textures";
		out << YAML::Value << YAML::BeginSeq;
		{
			for ( auto it = m_Material->m_Textures.begin(); it != m_Material->m_Textures.end(); ++it )
			{
				out << YAML::Flow;
				out << YAML::BeginSeq << it->first << it->second << YAML::EndSeq;
			}
		}
		out << YAML::EndSeq;

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

		m_Material->m_Handle = data["Material"].as<MaterialHandle>();
		m_Material->m_Shader = ShaderLibrary::GetShaderHandle( data["Shader"].as<std::string>() );
		m_Material->m_Parent = data["Parent Material"].as<MaterialHandle>();

		m_Material->BlendMode = data["Blend Mode"].as<EBlendMode>();
		m_Material->Color = data["Color"].as<Color>();
		m_Material->Reflectivity = data["Reflectivity"].as<float>();
		m_Material->Refraction = data["Refraction"].as<float>();
									  
		m_Material->BaseColorTexture = GetTexture( data["BaseColorTexture"].as<std::string>() );
		m_Material->NormalMapTexture = GetTexture( data["NormalMapTexture"].as<std::string>() );
		m_Material->MetallicTexture = GetTexture( data["MetallicTexture"].as<std::string>() );
		m_Material->RoughnessTexture = GetTexture( data["RoughnessTexture"].as<std::string>() );
		m_Material->EmissiveTexture = GetTexture( data["EmissiveTexture"].as<std::string>() );

		TE_CORE_TRACE( "End Deserializing Material" );

		return true;
	}

}