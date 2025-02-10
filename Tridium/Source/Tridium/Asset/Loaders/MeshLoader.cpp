#include "tripch.h"
#include "MeshLoader.h"

#include <Tridium/Graphics/Rendering/Buffer.h>
#include <Tridium/Graphics/Rendering/VertexArray.h>

#include "AssimpImporter.h"
#include "yaml-cpp/yaml.h"
#include <fstream>

namespace Tridium {

	SharedPtr<Asset> MeshSourceLoader::LoadAsset( const AssetMetaData& a_MetaData )
	{
		AssimpImporter importer( a_MetaData.Path );
		SharedPtr<MeshSource> meshSource = importer.ImportMeshSource();
		if ( !meshSource )
			return nullptr;

		meshSource->SetHandle( a_MetaData.Handle );
		return SharedPtrCast<Asset>( meshSource );
	}

	void StaticMeshLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
	{
		SharedPtr<StaticMesh> staticMesh = SharedPtrCast<StaticMesh>( a_Asset );
		TE_CORE_ASSERT( staticMesh );

		YAML::Emitter out;
		out << YAML::BeginMap; // StaticMesh

		out << YAML::Key << "MeshSource" << YAML::Value << staticMesh->GetMeshSource().ID();

		out << YAML::Key << "SubMeshes";
		out << YAML::Value << YAML::Flow << YAML::BeginSeq; // SubMeshes
		for ( const auto& subMesh : staticMesh->GetSubMeshes() )
			out << YAML::Value << subMesh;
		out << YAML::EndSeq; // SubMeshes

		out << YAML::Key << "Materials";
		out << YAML::Value << YAML::BeginSeq; // Materials
		for ( const auto& material : staticMesh->GetMaterials() )
			out << YAML::Value << material.ID();
		out << YAML::EndSeq; // Materials

		out << YAML::EndMap; // StaticMesh

		std::ofstream file( a_MetaData.Path.ToString() );
		file << out.c_str();
		file.close();
	}

	SharedPtr<Asset> StaticMeshLoader::LoadAsset( const AssetMetaData& a_MetaData )
	{
		YAML::Node node;
		try
		{
			node = YAML::LoadFile( a_MetaData.Path.ToString() );
		}
		catch ( const YAML::BadFile& e )
		{
			return nullptr;
		}

		AssetHandle meshSourceHandle = AssetHandle( node["MeshSource"].as<AssetHandle::Type>() );
		std::vector<uint32_t> subMeshes;
		if ( auto subMeshesNode = node["SubMeshes"] )
		{
			subMeshes = subMeshesNode.as<std::vector<uint32_t>>();
		}
		else
		{
			LOG( LogCategory::Asset, Error, "Failed to load sub - meshes from file : {0}", a_MetaData.Path.ToString() );
			return nullptr;
		}

		std::vector<AssetHandle::Type> materials;
		if ( auto materialsNode = node["Materials"] )
		{
			materials = materialsNode.as<std::vector<AssetHandle::Type>>();
		}
		else
		{
			LOG( LogCategory::Asset, Error, "Failed to load materials for static mesh from file : {0}", a_MetaData.Path.ToString() );
			return nullptr;
		}

		SharedPtr<StaticMesh> staticMesh = MakeShared<StaticMesh>( meshSourceHandle, subMeshes );
		for ( const auto& material : materials )
			staticMesh->GetMaterials().push_back( MaterialHandle( material ) );

		return SharedPtrCast<Asset>( staticMesh );
	}
}