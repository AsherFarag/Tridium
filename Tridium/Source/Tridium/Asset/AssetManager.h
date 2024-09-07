#pragma once
#include  "Asset.h"
#include <Tridium/IO/FilePath.h>

namespace Tridium {

	class AssetManager
	{
		friend class Application;
	public:
		static const AssetManager* Get() { return s_Instance; }

		static bool HasAsset( const AssetHandle& a_AssetHandle ) { return s_Instance->HasAssetImpl( a_AssetHandle ); }

		static AssetRef<Asset> GetAsset( const AssetHandle& a_AssetHandle, bool a_ShouldLoad = true );
		static AssetRef<Asset> GetAsset( const IO::FilePath& a_Path, bool a_ShouldLoad = true );

		static AssetRef<Asset> LoadAsset( const AssetHandle& a_AssetHandle );
		static AssetRef<Asset> LoadAsset( const IO::FilePath& a_Path );

		static bool ReleaseAsset( const AssetHandle& a_AssetHandle ) { return s_Instance->ReleaseAssetImpl( a_AssetHandle ); }

		template <typename T>
		static AssetRef<T> GetAsset( const AssetHandle& a_AssetHandle, bool a_ShouldLoad = true ) { return GetAsset( a_AssetHandle, a_ShouldLoad ).As<T>(); }
		template <typename T>
		static AssetRef<T> GetAsset( const IO::FilePath& a_Path, bool a_ShouldLoad = true ) { return GetAsset( a_Path, a_ShouldLoad ).As<T>(); }

		template <typename T>
		static AssetRef<T> LoadAsset( const AssetHandle& a_AssetHandle ) { return LoadAsset( a_AssetHandle ).As<T>(); }
		template <typename T>
		static AssetRef<T> LoadAsset( const IO::FilePath& a_Path ) { return LoadAsset( a_Path ).As<T>(); }

	protected:
		virtual bool HasAssetImpl( const AssetHandle& a_AssetHandle ) const = 0;
		virtual AssetRef<Asset> GetAssetImpl( const AssetHandle& a_AssetHandle ) = 0;
		virtual AssetRef<Asset> GetAssetImpl( const IO::FilePath& a_Path ) = 0;
		virtual AssetRef<Asset> LoadAssetImpl( const AssetHandle& a_AssetHandle ) = 0;
		virtual AssetRef<Asset> LoadAssetImpl( const IO::FilePath& a_Path ) = 0;
		virtual bool ReleaseAssetImpl( const AssetHandle& a_AssetHandle ) = 0;

	protected:
		static AssetManager* s_Instance;
	};
}