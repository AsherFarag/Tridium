#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Singleton.h>
#include "Asset.h"

#define ASSET_MANAGER_FILENAME "AssetManager.tmeta"

namespace Tridium {

	class AssetManager : public Singleton<AssetManager>
	{
	public:
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
		static AssetRef<T> GetAsset( const std::string& a_Path, bool a_ShouldLoad = true );

		template <typename T>
		static AssetRef<T> LoadAsset( const AssetHandle& a_AssetHandle );
		template <typename T>
		static AssetRef<T> LoadAsset( const std::string& a_Path );

		template <typename T>
		static bool AsyncLoadAsset( const AssetHandle& a_AssetHandle, AssetRef<T>& outAsset ) { return nullptr; }
		template <typename T>
		static bool AsyncLoadAsset( const std::string& a_Path, AssetRef<T>& outAsset ) { return nullptr; }

		/* Releases the AssetRef to a_AssetHandle and removes it from the asset library */
		static void ReleaseAsset( const AssetHandle& a_AssetHandle );

		static void Serialize( const fs::path& a_Path );
		bool Deserialize( const std::string& a_Path );

	private:
		/* Deserializes the Asset Directory Meta File containing a map of AssetHandle's to file paths and adds them to m_Paths. */
		virtual void Init() override;
		void Internal_AddAsset( const AssetRef<Asset>& asset );
		AssetRef<Asset> Internal_GetAsset( const AssetHandle& a_AssetHandle );
		const std::string& Internal_GetPath( const AssetHandle& a_AssetHandle );
		const AssetHandle& Internal_GetAssetHandle( const std::string& a_Path );
		bool Internal_RemoveAsset( const AssetHandle& a_AssetHandle );

	private:
		std::unordered_map<AssetHandle, AssetRef<Asset>> m_Library;
		std::unordered_map<AssetHandle, std::string> m_Paths;
	};

	template<typename T>
	bool AssetManager::HasAsset( const AssetHandle& a_AssetHandle )
	{
		if ( auto it = Get().m_Library.find( a_AssetHandle ); it != Get().m_Library.end() )
		{
			return it->second->GetAssetType() = T::GetStaticType();
		}

		return false;
	}

	template<typename T>
	inline AssetRef<T> AssetManager::GetAsset( const AssetHandle& a_AssetHandle, bool shouldLoad )
	{
		// Check if the asset is already loaded
		if ( auto asset = Get().Internal_GetAsset() )
		{
			if ( it->second->GetAssetType() != T::GetStaticType() )
			{
				TE_CORE_WARN( "Attempted to get asset of a type, but the asset's type is different" );
				return nullptr;
			}

			return std::static_pointer_cast<T>( it->second );
		}

		if ( !shouldLoad )
			return nullptr;

		return LoadAsset<T>( a_AssetHandle );
	}

	template<typename T>
	inline AssetRef<T> AssetManager::GetAsset( const std::string& a_Path, bool a_ShouldLoad )
	{
		return GetAsset<T>();
	}

	template<typename T>
	inline AssetRef<T> AssetManager::LoadAsset( const AssetHandle& a_AssetHandle )
	{
		if ( auto it = Get().m_Paths.find( a_AssetHandle ); it != Get().m_Paths.end() )
			return LoadAsset<T>( it->second );

		TE_CORE_WARN( "Could not find Asset Path for {0}", (AssetHandle::Type)a_AssetHandle.ID() );
		return nullptr;
	}

	template<typename T>
	inline AssetRef<T> AssetManager::LoadAsset( const std::string& a_Path )
	{
		return T::Load( a_Path );
	}

}