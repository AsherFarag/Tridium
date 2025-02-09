#pragma once
#include <Tridium/Core/Memory.h>
#include <Tridium/Asset/AssetMetaData.h>

namespace Tridium {
	class FilePath;
	class IAssetLoader;
	struct AssetMetaData;

	class AssetFactory
	{
	public:
		static void Init();
		static void SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset );
		static SharedPtr<Asset> LoadAsset( const AssetMetaData& a_MetaData );

		static SharedPtr<IAssetLoader> GetAssetLoader( EAssetType a_Type );

		template<typename T>
		static SharedPtr<IAssetLoader> GetAssetLoader()
		{
			static_assert( std::is_base_of_v<Asset, T>, "T must inherit from Asset" );
			return GetAssetLoader( T::StaticType() );
		}

	private:
		static std::unordered_map<EAssetType, SharedPtr<IAssetLoader>> s_AssetFactories;
	};

}