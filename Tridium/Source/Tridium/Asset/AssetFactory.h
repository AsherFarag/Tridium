#pragma once
#include "Asset.h"
#include <Tridium/Asset/Loaders/AssetLoader.h>
#include <unordered_map>
#include <Tridium/IO/FilePath.h>

namespace Tridium {

	class AssetFactory
	{
		static void RegisterLoader( EAssetType a_AssetType, IAssetLoader& a_AssetLoader ) 
		{
			m_Loaders[a_AssetType] = &a_AssetLoader;
		}

		static IAssetLoader* GetLoader( EAssetType a_AssetType )
		{
			auto it = m_Loaders.find( a_AssetType );
			return it != m_Loaders.end() ? it->second : nullptr;
		}

		static AssetRef<Asset> LoadAsset( EAssetType a_AssetType, const IO::FilePath& a_Path )
		{
			IAssetLoader* loader = GetLoader( a_AssetType );
			return loader ? loader->Load( a_Path ) : nullptr;
		}

		template<typename T>
		static IAssetLoader* GetLoader()
		{
			return GetLoader( T::StaticType() );
		}

		template<typename T>
		static AssetRef<T> LoadAsset( const IO::FilePath& a_Path )
		{
			return LoadAsset( T::StaticType() );
		}

	private:
		static inline std::unordered_map<EAssetType, IAssetLoader*> m_Loaders;
	};

}