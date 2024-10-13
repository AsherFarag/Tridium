#include "tripch.h"
#include "MaterialLoader.h"
#include <Tridium/IO/Serializer.h>

namespace Tridium {

	AssetMetaData* MaterialLoader::LoadAssetMetaData( const YAML::Node & a_Node ) const
	{
		Scope<ModelMetaData> metaData = new ModelMetaData();
		if ( !IO::DeserializeFromArchive( a_Node, *metaData ) )
			return nullptr;

		metaData.Retire();
		return metaData.Get();
	}
	AssetMetaData* MaterialLoader::ConstructAssetMetaData() const
	{
		return new ModelMetaData();
	}
	Asset* MaterialLoader::RuntimeLoad( const IO::FilePath& a_Path ) const
	{
		return nullptr;
	}
	Asset* MaterialLoader::DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const
	{
		return nullptr;
	}
	bool MaterialLoader::Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const
	{
		return false;
	}
}