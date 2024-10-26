#include "tripch.h"
#include "MaterialLoader.h"
#include <Tridium/IO/Serializer.h>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/EditorAssetManager.h>

namespace Tridium {

	void MaterialLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
	{
		SharedPtr<Material> material = SharedPtrCast<Material>(a_Asset);
		TE_CORE_ASSERT( material );

		TODO( "Should we be adding dependencies here?" );
		AssetManager::RegisterDependency( a_MetaData.Handle, material->AlbedoTexture );
		AssetManager::RegisterDependency( a_MetaData.Handle, material->MetallicTexture );
		AssetManager::RegisterDependency( a_MetaData.Handle, material->RoughnessTexture );
		AssetManager::RegisterDependency( a_MetaData.Handle, material->SpecularTexture );
		AssetManager::RegisterDependency( a_MetaData.Handle, material->NormalTexture );
		AssetManager::RegisterDependency( a_MetaData.Handle, material->OpacityTexture );
		AssetManager::RegisterDependency( a_MetaData.Handle, material->EmissiveTexture );
		AssetManager::RegisterDependency( a_MetaData.Handle, material->AOTexture );

		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Shader" << YAML::Value << material->Shader.ID();
			out << YAML::Key << "AlbedoTexture" << YAML::Value << material->AlbedoTexture.ID();
			out << YAML::Key << "MetallicTexture" << YAML::Value << material->MetallicTexture.ID();
			out << YAML::Key << "RoughnessTexture" << YAML::Value << material->RoughnessTexture.ID();
			out << YAML::Key << "SpecularTexture" << YAML::Value << material->SpecularTexture.ID();
			out << YAML::Key << "NormalTexture" << YAML::Value << material->NormalTexture.ID();
			out << YAML::Key << "OpacityTexture" << YAML::Value << material->OpacityTexture.ID();
			out << YAML::Key << "EmissiveTexture" << YAML::Value << material->EmissiveTexture.ID();
			out << YAML::Key << "AOTexture" << YAML::Value << material->AOTexture.ID();
		}
		out << YAML::EndMap;

		TODO( "This is editor only!" );
		std::ofstream file(  AssetManager::Get<Editor::EditorAssetManager>()->GetAbsolutePath( a_MetaData.Path ).ToString() );
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