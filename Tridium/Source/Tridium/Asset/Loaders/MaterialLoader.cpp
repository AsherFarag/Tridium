#include "tripch.h"
#include "MaterialLoader.h"
#include <Tridium/IO/Serializer.h>
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Tridium {

	void MaterialLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
	{
		SharedPtr<Material> material = SharedPtrCast<Material>(a_Asset);
		TE_CORE_ASSERT( material );

		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Shader" << YAML::Value << material->Shader.ID();
			out << YAML::Key << "Albedo" << YAML::Value << material->AlbedoTexture.ID();
			out << YAML::Key << "Metallic" << YAML::Value << material->MetallicTexture.ID();
			out << YAML::Key << "Roughness" << YAML::Value << material->RoughnessTexture.ID();
			out << YAML::Key << "Specular" << YAML::Value << material->SpecularTexture.ID();
			out << YAML::Key << "Normal" << YAML::Value << material->NormalTexture.ID();
			out << YAML::Key << "Opacity" << YAML::Value << material->OpacityTexture.ID();
			out << YAML::Key << "Emissive" << YAML::Value << material->EmissiveTexture.ID();
			out << YAML::Key << "AO" << YAML::Value << material->AOTexture.ID();
		}
		out << YAML::EndMap;

		std::ofstream file( a_MetaData.Path.ToString() );
		file << out.c_str();
	}

	SharedPtr<Asset> MaterialLoader::LoadAsset( const AssetMetaData& a_MetaData )
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile( a_MetaData.Path.ToString() );
		}
		catch ( const YAML::BadFile& e )
		{
			return nullptr;
		}

#define LOAD_MATERIAL_PROPERTY( prop ) \
		if ( auto propNode = data[#prop] ) \
		{\
			material->prop = propNode.as<AssetHandle::Type>(); \
		} \
		else \
		{\
			return nullptr; \
		}

		SharedPtr<Material> material = MakeShared<Material>();
		LOAD_MATERIAL_PROPERTY( Shader );
		LOAD_MATERIAL_PROPERTY( AlbedoTexture );
		LOAD_MATERIAL_PROPERTY( MetallicTexture );
		LOAD_MATERIAL_PROPERTY( RoughnessTexture );
		LOAD_MATERIAL_PROPERTY( SpecularTexture );
		LOAD_MATERIAL_PROPERTY( NormalTexture );
		LOAD_MATERIAL_PROPERTY( OpacityTexture );
		LOAD_MATERIAL_PROPERTY( EmissiveTexture );
		LOAD_MATERIAL_PROPERTY( AOTexture );

		return material;
	}
}