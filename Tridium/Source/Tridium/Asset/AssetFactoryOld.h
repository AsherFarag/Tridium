#pragma once
#include <Tridium/Core/Memory.h>
#include <Tridium/Asset/AssetMetaData.h>

namespace Tridium {
	class FilePath;
	class IAssetLoaderOld;
	struct AssetMetaData;

	class AssetFactoryOld
	{
	public:
		static void Init();
		static void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset );
		static SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData );

		static SharedPtr<IAssetLoaderOld> GetAssetLoader( EAssetType a_Type );

		template<typename T>
		static SharedPtr<IAssetLoaderOld> GetAssetLoader()
		{
			static_assert( std::is_base_of_v<Asset, T>, "T must inherit from Asset" );
			return GetAssetLoader( T::StaticType() );
		}

	private:
		static std::unordered_map<EAssetType, SharedPtr<IAssetLoaderOld>> s_AssetFactories;
	};

}