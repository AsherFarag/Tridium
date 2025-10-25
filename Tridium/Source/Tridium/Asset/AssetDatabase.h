#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Common/TimeStamp.h>
#include <Tridium/Containers/UnorderedSet.h>
#include <Tridium/Core/Core.h>

namespace Tridium {

	//=================================================================================================
	// Asset Metadata: Serialized metadata for an asset.
	//=================================================================================================
	struct AssetMetadata
	{
		AssetID ID = InvalidAssetID;
		AssetTypeID Type = InvalidAssetTypeID;
		EAssetLoadPolicy LoadPolicy = EAssetLoadPolicy::Default;
		String Name{};
		String Path{};
		UnorderedSet<AssetID> Dependencies{};

	#if WITH_EDITOR

		struct
		{
			// The original source file path from which the asset was imported from.
			String SourceFilePath{};
			// The timestamp of when the asset was created.
			TimeStamp CreationTime{};
			// The timestamp of when the asset was last modified.
			TimeStamp LastModifiedTime{};
		} Editor{};

	#endif

		bool Valid() const { return ID != InvalidAssetID && Type != InvalidAssetTypeID; }
		static AssetMetadata From( IAsset& a_Asset ) { return { .ID = a_Asset.ID(), .Type = a_Asset.Type() }; }
	};

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
		static AssetID GetAssetIDFromPath( StringView a_Path );
		// Retrieves the asset path from an AssetID.
		static StringView GetAssetPathFromID( AssetID a_AssetID );
		// Retrieves the AssetMetadata for a given AssetID.
		static const AssetMetadata* GetAssetMetadata( AssetID a_AssetID );

		//=============================================================================================
		// Retrieves an asset by its AssetID if it exists and is loaded.
		static AssetHandle<IAsset> GetAsset( AssetID a_AssetID );
		// Retrieves an asset by its AssetID, loading it if it is not already loaded.
		static AssetHandle<IAsset> GetOrLoadAsset( AssetID a_AssetID );
		// Returns true if the asset exists and is valid.
		static bool IsAssetLoaded( AssetID a_AssetID );
		// Returns true if the asset exists in the database, regardless of whether it is loaded or not.
		static bool DoesAssetExist( AssetID a_AssetID );

		//=============================================================================================
		// Returns the asset of type T if it exists and is loaded.
		template<Concepts::Derived<IAsset> T>
		static AssetHandle<T> GetAsset( AssetID a_AssetID ) { return GetAsset( a_AssetID ).Cast<T>(); }
		// Returns the asset of type T if it exists and is loaded, or loads it if not already loaded.
		template<Concepts::Derived<IAsset> T>
		static AssetHandle<T> GetOrLoadAsset( AssetID a_AssetID ) { return GetOrLoadAsset( a_AssetID ).Cast<T>(); }

		//=============================================================================================
		// Registers an asset to the database.
		static bool RegisterAsset( SharedPtr<IAsset> a_Asset, AssetMetadata a_Metadata );
		// Unregisters an asset from the database.
		static bool UnregisterAsset( AssetID a_AssetID );
		// Registers 'a_Dependency' as a dependency of 'a_Dependant' asset into the database.
		static void RegisterDependency( AssetID a_Dependant, AssetID a_Dependency );
		// Unregisters 'a_Dependency' as a dependency of 'a_Dependant' asset from the database.
		static void UnregisterDependency( AssetID a_Dependant, AssetID a_Dependency );

	#if WITH_EDITOR

		//=============================================================================================
		// Imports and creates an asset from the specified path and returns the created AssetID.
		static AssetID ImportAsset( StringView a_Path );
		// Registers the asset to the database and serializes it to disk.
		static bool CreateAsset( IAsset* a_Asset, StringView a_Path );
		// Unregisters an asset from the database and deletes it from disk.
		static bool DeleteAsset( AssetID a_AssetID );

	#endif

		//=============================================================================================
		// Returns the name of the asset.
		static StringView GetAssetName( AssetID a_AssetID );

	private:

		//=============================================================================================
		UnorderedMap<String, AssetID> m_AssetPathMap;
		UnorderedMap<AssetID, Pair<AssetMetadata, SharedPtr<IAsset>>> m_Assets;

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
		if ( !m_Ref && m_ID != InvalidAssetID )
		{
			*this = AssetDatabase::GetOrLoadAsset<T>( m_ID );
		}

		return m_Ref;
	}

} // namespace Tridium