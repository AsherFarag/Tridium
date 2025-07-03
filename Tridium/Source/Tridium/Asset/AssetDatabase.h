#pragma once
#include "Asset.h"
#include "AssetFactory.h"
#include <Tridium/Core/Core.h>
#include <Tridium/Common/TimeStamp.h>

namespace Tridium::T {

	enum class EAssetImporterType
	{
		Unknown = 0,
		Texture,
		Model,
		Audio,
		Video,
		Shader,
		Script,
	};

	//=====================
	// Asset Metadata
	//  Serialized metadata for an asset.
	struct AssetMetadata
	{
		AssetID ID = AssetID::InvalidID;
		EAssetType Type = EAssetType::Unknown;
		EAssetImporterType ImporterType = EAssetImporterType::Unknown;
		EAssetLoadPolicy LoadPolicy = EAssetLoadPolicy::Default;
		String Name{};
		String Path{};
		UnorderedSet<AssetID> Dependencies{};

	#if WITH_EDITOR
		struct
		{
			TimeStamp CreationTime{};
			TimeStamp LastModifiedTime{};
		} Editor{}; // Editor-specific metadata
	#endif

		bool Valid() const { return ID != AssetID::InvalidID && Type != EAssetType::Unknown; }
		static AssetMetadata From( IAsset& a_Asset ) { return { .ID = a_Asset.ID(), .Type = a_Asset.Type() }; }
	};

}

namespace Tridium {

	//==============================================================================
	// Asset Database
	//  This class is responsible for
	class AssetDatabase
	{
	public:
		NON_COPYABLE_OR_MOVABLE( AssetDatabase );
		static AssetDatabase* Get() { return s_Instance; }

		// Retrieves the AssetID of an asset based on its path.
		static AssetID GetAssetIDFromPath( StringView a_Path );
		// Retrieves the asset path from an AssetID.
		static StringView GetAssetPathFromID( AssetID a_AssetID );
		// Retrieves the AssetMetadata for a given AssetID.
		static const T::AssetMetadata* GetAssetMetadata( AssetID a_AssetID );

		// Retrieves an asset by its AssetID if it exists and is loaded.
		static IAsset* GetAsset( AssetID a_AssetID );
		// Retrieves an asset by its AssetID, loading it if it is not already loaded.
		static IAsset* GetOrLoadAsset( AssetID a_AssetID );
		// Returns true if the asset exists and is valid.
		static bool IsAssetLoaded( AssetID a_AssetID );
		// Returns true if the asset exists in the database, regardless of whether it is loaded or not.
		static bool DoesAssetExist( AssetID a_AssetID );

		// Returns the asset of type T if it exists and is loaded.
		template<Concepts::Derived<IAsset> T>
		static T* GetAsset( AssetID a_AssetID ) { return DynamicCast<T*>( GetAsset( a_AssetID ) ); }
		// Returns the asset of type T if it exists and is loaded, or loads it if not already loaded.
		template<Concepts::Derived<IAsset> T>
		static T* GetOrLoadAsset( AssetID a_AssetID ) { return DynamicCast<T*>( GetOrLoadAsset( a_AssetID ) ); }

		// Registers an asset to the database.
		static bool RegisterAsset( const SharedPtr<IAsset>& a_Asset, const T::AssetMetadata& a_Metadata );
		// Unregisters an asset from the database.
		static bool UnregisterAsset( AssetID a_AssetID );
		// Registers 'a_Dependency' as a dependency of 'a_Dependant' asset into the database.
		static void RegisterDependency( AssetID a_Dependant, AssetID a_Dependency );
		// Unregisters 'a_Dependency' as a dependency of 'a_Dependant' asset from the database.
		static void UnregisterDependency( AssetID a_Dependant, AssetID a_Dependency );

	#if WITH_EDITOR
		// Editor-only functions for asset management.
		struct Editor
		{
			// Imports and creates an asset from the specified path and returns the created AssetID.
			static AssetID ImportAsset( StringView a_Path );
			// Registers the asset to the database and serializes it to disk.
			static bool CreateAsset( IAsset* a_Asset, StringView a_Path );
			// Unregisters an asset from the database and deletes it from disk.
			static bool DeleteAsset( AssetID a_AssetID );
		};
	#endif

		// = Utility Functions =

		// Returns the name of the asset, if the asset is not named or does not exist, returns '<UNKNOWN>'.
		static StringView GetAssetName( AssetID a_AssetID );

	private:
		UnorderedMap<String, AssetID, TransparentStringHash, std::equal_to<>> m_AssetPathMap;
		UnorderedMap<AssetID, Pair<T::AssetMetadata, SharedPtr<IAsset>>> m_Assets;
		T::AssetFactory m_AssetFactory;

	private:
		AssetDatabase() = default;
		~AssetDatabase() = default;

		static AssetDatabase* s_Instance;
		static Expected<void, String> Init();
		static Expected<void, String> Shutdown();

		friend struct Editor;
		friend class Engine;
	};

} // namespace Tridium