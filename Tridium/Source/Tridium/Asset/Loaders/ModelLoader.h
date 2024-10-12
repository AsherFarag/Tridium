#pragma once
#include "AssetLoader.h"
#include <Tridium/Rendering/Mesh.h>

namespace Tridium {

	class ModelLoader : public IAssetLoader
	{
		// Inherited via IAssetLoader
		AssetMetaData* LoadAssetMetaData( const YAML::Node& a_Node ) const override;
		AssetMetaData* ConstructAssetMetaData() const override;
		Asset* RuntimeLoad( const IO::FilePath& a_Path ) const override;
		Asset* DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const override;
		bool Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const override;
	};

}