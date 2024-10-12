#include "tripch.h"
#include "MaterialLoader.h"
#include <Tridium/IO/MaterialSerializer.h>

namespace Tridium {

	AssetMetaData* MaterialLoader::LoadAssetMetaData( const YAML::Node & a_Node ) const
	{
		return nullptr;
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