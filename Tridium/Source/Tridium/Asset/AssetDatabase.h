#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Core.h>

namespace Tridium {

	//=================================================================================================
	// Asset Database: Global, centralized database for managing assets.
	// This class is responsible for importing, loading, unloading,
	// and keeping track of all assets in the game.
	//=================================================================================================
	class AssetDatabase final
	{
	public:

		//=============================================================================================
		NON_COPYABLE_OR_MOVABLE( AssetDatabase );

		//=============================================================================================
		// Returns the singleton instance of the AssetDatabase.
		static AssetDatabase* Get() { return s_Instance; }

		//=============================================================================================
		// Retrieves the AssetID of an asset based on its path.
		static UUID GetAssetIDFromPath( StringView a_Path );
		// Retrieves the asset path from an AssetID.
		static StringView GetAssetPathFromID( UUID a_AssetID );
		// Retrieves the AssetInfo for a given AssetID.
		static const AssetInfo* GetAssetInfo( UUID a_AssetID );

		//=============================================================================================
		// Retrieves an asset by its AssetID if it exists and is loaded.
		static AssetHandle<IAsset> GetAsset( UUID a_AssetID );
		// Retrieves an asset by its AssetID, loading it if it is not already loaded.
		static AssetHandle<IAsset> GetOrLoadAsset( UUID a_AssetID );
		// Returns true if the asset exists and is valid.
		static bool IsAssetLoaded( UUID a_AssetID );
		// Returns true if the asset exists in the database, regardless of whether it is loaded or not.
		static bool DoesAssetExist( UUID a_AssetID );

		//=============================================================================================
		// Returns the asset of type T if it exists and is loaded.
		template<Concepts::Derived<IAsset> T>
		static AssetHandle<T> GetAsset( UUID a_AssetID ) { return GetAsset( a_AssetID ).Cast<T>(); }
		// Returns the asset of type T if it exists and is loaded, or loads it if not already loaded.
		template<Concepts::Derived<IAsset> T>
		static AssetHandle<T> GetOrLoadAsset( UUID a_AssetID ) { return GetOrLoadAsset( a_AssetID ).Cast<T>(); }

		//=============================================================================================
		// Registers an asset to the database.
		static bool RegisterAsset( SharedPtr<IAsset> a_Asset );
		// Unregisters an asset from the database.
		static bool UnregisterAsset( UUID a_AssetID );
		// Registers 'a_Dependency' as a dependency of 'a_Dependant' asset into the database.
		static void RegisterDependency( UUID a_Dependant, UUID a_Dependency );
		// Unregisters 'a_Dependency' as a dependency of 'a_Dependant' asset from the database.
		static void UnregisterDependency( UUID a_Dependant, UUID a_Dependency );

	#if WITH_EDITOR

		//=============================================================================================
		// Imports and creates an asset from the specified path and returns the created AssetID.
		static UUID ImportAsset( StringView a_Path );
		// Registers the asset to the database and serializes it to disk.
		static bool CreateAsset( IAsset* a_Asset, StringView a_Path );
		// Unregisters an asset from the database and deletes it from disk.
		static bool DeleteAsset( UUID a_AssetID );

	#endif

		//=============================================================================================
		// Returns the name of the asset.
		static StringView GetAssetName( UUID a_AssetID );

	private:

		//=============================================================================================
		UnorderedMap<String, UUID> m_AssetPathMap;
		UnorderedMap<UUID, Pair<SharedPtr<AssetInfo>, SharedPtr<IAsset>>> m_Assets;

		//=============================================================================================
		AssetDatabase() = default;
		~AssetDatabase() = default;

		//=============================================================================================
		static AssetDatabase* s_Instance;
		Expected<void, String> Init();
		Expected<void, String> Shutdown();

		friend class Project;
	};

	//=================================================================================================
	template<Concepts::Derived<IAsset> T>
	inline const AssetRef<T>& AssetHandle<T>::GetOrLoad()
	{
		if ( !m_Ref && m_ID.Valid() )
		{
			*this = AssetDatabase::GetOrLoadAsset<T>( m_ID );
		}

		return m_Ref;
	}

} // namespace Tridium