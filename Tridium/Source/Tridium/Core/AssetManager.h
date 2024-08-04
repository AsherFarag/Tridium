#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Singleton.h>
#include "Asset.h"

#define ASSET_MANAGER_FILENAME "AssetManager.tmeta"

namespace Tridium {

	class AssetManager final : public Singleton<AssetManager>
	{
	public:
		auto& GetLibrary() { return m_Library; }

		/* Returns true if an asset with this GUID is loaded in the asset library. */
		template <typename T>
		static bool HasAsset( const GUID& a_AssetGUID );

		// Returns the loaded asset with the corresponding GUID. 
		// If the asset isn't loaded and a_ShouldLoad == true, the asset will be loaded and returned.
		// else, returns nullptr.
		template <typename T>
		static AssetRef<T> GetAsset( const GUID& a_AssetGUID, bool a_ShouldLoad = true );

		template <typename T>
		static AssetRef<T> LoadAsset( const GUID& a_AssetGUID );
		template <typename T>
		static AssetRef<T> LoadAsset( const std::string& a_Path );

		template <typename T>
		static bool AsyncLoadAsset( const GUID& a_AssetGUID, AssetRef<T>& outAsset ) { return nullptr; }
		template <typename T>
		static bool AsyncLoadAsset( const std::string& a_Path, AssetRef<T>& outAsset ) { return nullptr; }

		static void Serialize( const fs::path& a_Path );
		bool Deserialize( const std::string& a_Path );
		static AssetRef<Asset> Import( const fs::path& a_Path );

	private:
		/* Deserializes the Asset Directory Meta File containing a map of GUID's to file paths and adds them to m_Paths. */
		virtual void Init() override;
		static void AddAsset( const AssetRef<Asset>& asset ) { Get().m_Library[asset->GetGUID()] = asset; }

	private:
		std::unordered_map<GUID, AssetRef<Asset>> m_Library;
		std::unordered_map<GUID, std::string> m_Paths;
	};

	template<typename T>
	bool AssetManager::HasAsset( const GUID& a_AssetGUID )
	{
		if ( auto it = Get().m_Library.find( a_AssetGUID ); it != Get().m_Library.end() )
		{
			if ( it->second->GetAssetType() != T::GetStaticType() )
			{
				return false;
			}

			return true;
		}

		return false;
	}

	template<typename T>
	inline AssetRef<T> AssetManager::GetAsset( const GUID& a_AssetGUID, bool shouldLoad )
	{
		// Check if the asset is already loaded
		if ( auto it = Get().m_Library.find( a_AssetGUID ); it != Get().m_Library.end() )
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

		//AssetRef<T> asset = MakeShared<T>();
		//asset->m_GUID = a_a_AssetGUID;
		//TODO( "Add asynchronous loading." );
		//if ( !LoadAsset( a_a_AssetGUID, asset ) )
		//	return false;

		return LoadAsset<T>( a_AssetGUID );
	}

	template<typename T>
	inline AssetRef<T> AssetManager::LoadAsset( const GUID& a_AssetGUID )
	{
		if ( auto it = Get().m_Paths.find( a_AssetGUID ); it != Get().m_Paths.end() )
			return LoadAsset<T>( it->second );

		TE_CORE_WARN( "Could not find Asset Path for {0}", (GUID::Type)a_AssetGUID.ID() );
		return nullptr;
	}

	template<typename T>
	inline AssetRef<T> AssetManager::LoadAsset( const std::string& a_Path )
	{
		return T::Load( a_Path );
	}

}