#pragma once
#include <Tridium/Asset/AssetType.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/AssetMetaData.h>
#include <Tridium/IO/FilePath.h>
#include <Tridium/IO/Archive.h>

#define ASSET_USE_RUNTIME 0

namespace Tridium {

	class IAssetLoader
	{
	public:
		Asset* Load( const IO::FilePath& a_Path ) const
		{
        #if ASSET_USE_RUNTIME
			return RuntimeLoad( a_Path );
        #else
			return DebugLoad( a_Path, nullptr );
        #endif
		}

		virtual AssetMetaData* LoadAssetMetaData( const YAML::Node & a_Node ) const = 0;
		virtual AssetMetaData* ConstructAssetMetaData() const = 0;
		virtual Asset* RuntimeLoad( const IO::FilePath& a_Path ) const = 0;
		virtual Asset* DebugLoad( const IO::FilePath& a_Path, const AssetMetaData* a_MetaData ) const = 0;
		virtual bool Save( const IO::FilePath& a_Path, const Asset* a_Asset ) const = 0;
	};

}