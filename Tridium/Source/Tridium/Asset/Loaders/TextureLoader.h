#pragma once
#include "AssetLoader.h"
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	class TextureLoader : public IAssetLoader
	{
	public:
		static Texture* Load( const IO::FilePath& a_Path );

		virtual AssetMetaData* LoadAssetMetaData( const YAML::Node& a_Node ) const override;
		virtual AssetMetaData* ConstructAssetMetaData() const override;
		virtual Asset* RuntimeLoad( const IO::FilePath& a_Path ) const override;
		virtual Asset* DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const override;
		virtual bool Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const override;
	};

}