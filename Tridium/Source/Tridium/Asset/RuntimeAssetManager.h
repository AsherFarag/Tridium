#pragma once
#include "AssetManager.h"

namespace Tridium {

	class RuntimeAssetManager : public AssetManager
	{
	public:

		// Inherited via AssetManager
		bool HasAssetImpl( const AssetHandle& a_AssetHandle ) const override;
		AssetRef<Asset> GetAssetImpl( const AssetHandle& a_AssetHandle ) override;
		AssetRef<Asset> GetAssetImpl( const IO::FilePath& a_Path ) override;
		AssetRef<Asset> LoadAssetImpl( const AssetHandle& a_AssetHandle ) override;
		AssetRef<Asset> LoadAssetImpl( const IO::FilePath& a_Path ) override;
		bool ReleaseAssetImpl( const AssetHandle& a_AssetHandle ) override;
	};

}