#pragma once
#include <Tridium/Core/Core.h>
#include "Asset.h"

#define ASSET_MANAGER_FILENAME "AssetManager.tmeta"
#define META_FILE_EXTENSION ".meta"

namespace Tridium {

	class AssetManager
	{
		friend class Application;

	public:
		static SharedPtr<AssetManager> Get() { return s_Instance; }
		auto& GetLibrary() { return m_Library; }

		/* Returns true if an asset with this AssetHandle is loaded in the asset library. */
		template <typename T>
		static bool HasAsset( const AssetHandle& a_AssetHandle );

		// Returns the loaded asset with the corresponding AssetHandle. 
		// If the asset isn't loaded and a_ShouldLoad == true, the asset will be loaded and returned.
		// else, returns nullptr.
		template <typename T>
		static AssetRef<T> GetAsset( const AssetHandle& a_AssetHandle, bool a_ShouldLoad = true );

		// Returns the asset loaded from the filepath. 
		// If the asset isn't loaded and a_ShouldLoad == true, the asset will be loaded and returned.
		// else, returns nullptr.
		template <typename T>
		static AssetRef<T> GetAsset( const fs::path& a_Path, bool a_ShouldLoad = true );

		template <typename T>
		static AssetRef<T> LoadAsset( const AssetHandle& a_AssetHandle );
		template <typename T>
		static AssetRef<T> LoadAsset( const fs::path& a_Path );

		/* Releases the AssetRef to a_AssetHandle and removes it from the asset library */
		static void ReleaseAsset( const AssetHandle& a_AssetHandle );

		static void Serialize( const fs::path& a_Path );
		bool Deserialize( const fs::path& a_Path );

	protected:
		void Internal_AddAsset( const AssetRef<Asset>& a_Asset );
		AssetRef<Asset> Internal_LoadAsset( const AssetHandle& a_AssetHandle );
		AssetRef<Asset> Internal_LoadAsset( const fs::path& a_Path );
		AssetRef<Asset> Internal_GetAsset( const AssetHandle& a_AssetHandle );
		const fs::path& Internal_GetPath( const AssetHandle& a_AssetHandle );
		const AssetHandle& Internal_GetAssetHandle( const fs::path& a_Path );
		bool Internal_RemoveAsset( const AssetHandle& a_AssetHandle );

	protected:
		std::unordered_map<AssetHandle, AssetRef<Asset>> m_Library;
		std::unordered_map<AssetHandle, fs::path> m_Paths;

		static SharedPtr<AssetManager> s_Instance;
	};

#pragma region Template Definitions

	template<typename T>
	bool AssetManager::HasAsset( const AssetHandle& a_AssetHandle )
	{
		if ( auto it = Get()->m_Library.find( a_AssetHandle ); it != Get()->m_Library.end() )
		{
			return it->second->AssetType() = T::StaticType();
		}

		return false;
	}

	template<typename T>
	inline AssetRef<T> AssetManager::GetAsset( const AssetHandle& a_AssetHandle, bool a_ShouldLoad )
	{
		// Check if the asset is already loaded
		if ( auto asset = Get()->Internal_GetAsset( a_AssetHandle ) )
		{
			return asset.As<T>();
		}

		if ( !a_ShouldLoad )
			return nullptr;

		return LoadAsset<T>( a_AssetHandle );
	}

	template<typename T>
	inline AssetRef<T> AssetManager::GetAsset( const fs::path& a_Path, bool a_ShouldLoad )
	{
		return nullptr;
	}

	template<typename T>
	inline AssetRef<T> AssetManager::LoadAsset( const AssetHandle& a_AssetHandle )
	{
		if ( auto it = Get()->m_Paths.find( a_AssetHandle ); it != Get()->m_Paths.end() )
			return LoadAsset<T>( it->second );

		TE_CORE_WARN( "Could not find Asset Path for {0}", (AssetHandle::Type)a_AssetHandle.ID() );
		return nullptr;
	}

	template<typename T>
	inline AssetRef<T> AssetManager::LoadAsset( const fs::path& a_Path )
	{
		return Get()->Internal_LoadAsset(a_Path).As<T>();
	}

#pragma endregion

}