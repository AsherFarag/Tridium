#pragma once
#include "AssetLoader.h"
#include <Tridium/Rendering/Shader.h>

namespace Tridium {

	class ShaderLoader : public IAssetLoader
	{
	public:
		AssetMetaData* LoadAssetMetaData( const YAML::Node& a_Node ) const override;
		AssetMetaData* ConstructAssetMetaData() const override;
		Asset* RuntimeLoad( const IO::FilePath& a_Path ) const override;
		Asset* DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const override;
		bool Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const override;
	};

}