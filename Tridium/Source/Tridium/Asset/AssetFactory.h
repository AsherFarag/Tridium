#pragma once
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/AssetMetaData.h>
#include <unordered_map>
#include <Tridium/IO/Archive.h>

namespace Tridium {

	template<typename T>
	class AssetRef;
	struct AssetMetaData;
	class IO::FilePath;
	class IAssetLoader;

	class AssetFactory
	{
	public:
		static void InitializeLoaders();
		static void RegisterLoader( EAssetType a_AssetType, IAssetLoader& a_AssetLoader );
		static IAssetLoader* GetLoader( EAssetType a_AssetType );
		static AssetRef<Asset> LoadAsset( EAssetType a_AssetType, const IO::FilePath& a_Path );
		static bool SaveAsset( const IO::FilePath& a_Path, AssetRef<Asset> a_Asset );
		static AssetMetaData* LoadAssetMetaData( EAssetType a_AssetType, const YAML::Node& a_Node );
		static AssetMetaData* ConstructAssetMetaData( EAssetType a_AssetType );

		template<typename T>
		static IAssetLoader* GetLoader() { return GetLoader( T::StaticType() ); }

		template<typename T>
		static AssetRef<T> LoadAsset( const IO::FilePath& a_Path ) { return LoadAsset( T::StaticType(), a_Path ); }

		template<typename T>
		static bool SaveAsset( const IO::FilePath& a_Path, AssetRef<T> a_Asset ) { return SaveAsset( a_Path, a_Asset.As<Asset>() ); }

	private:
		static inline std::unordered_map<EAssetType, IAssetLoader*> m_Loaders;
	};

}