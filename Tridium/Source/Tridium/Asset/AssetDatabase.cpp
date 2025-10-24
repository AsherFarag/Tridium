#include "tripch.h"
#include "AssetDatabase.h"
#include <Tridium/Asset/AssetLoader.h>
#include <Tridium/Asset/AssetImporter.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/IO/FileIO.h>

namespace Tridium {

	AssetDatabase* AssetDatabase::s_Instance = nullptr;

	static constexpr StringView NameIfNotNull( StringView a_Name )
	{
		return a_Name.empty() ? "<UNNAMED>" : a_Name;
	}

	AssetID AssetDatabase::GetAssetIDFromPath( StringView a_Path )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		auto assetID = Get()->m_AssetPathMap.find(a_Path);

		if (assetID == Get()->m_AssetPathMap.end())
			return AssetID::InvalidID;

		return assetID->second;
	}

	StringView AssetDatabase::GetAssetPathFromID( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.IsValid() )
			return {};

		auto it = Get()->m_Assets.find( a_AssetID );

		if ( it == Get()->m_Assets.end() )
			return {};

		const AssetMetadata& metadata = it->second.first;
		return metadata.Path;
	}

	const AssetMetadata* AssetDatabase::GetAssetMetadata( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.IsValid() )
			return nullptr;

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it == Get()->m_Assets.end() )
			return nullptr;

		return &it->second.first;
	}

	AssetHandle<IAsset> AssetDatabase::GetAsset( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.IsValid() )
			return nullptr;

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it == Get()->m_Assets.end() )
			return nullptr; // Asset does not exist

		return it->second.second;
	}

	AssetHandle<IAsset> AssetDatabase::GetOrLoadAsset( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		auto assetIt = Get()->m_Assets.find( a_AssetID );

		if ( assetIt == Get()->m_Assets.end() )
		{
			// Asset does not exist
			return nullptr;
		}

		SharedPtr<IAsset>& asset = assetIt->second.second;
		if ( asset )
		{
			// Asset is already loaded
			return asset;
		}

		// The asset exists but is not loaded, attempt to load it.
		AssetMetadata& metadata = assetIt->second.first;
		IAssetLoader* loader = AssetFactory::GetLoader( metadata.Type );

		if ( loader == nullptr )
		{
			LOG( LogCategory::Asset, Error, "No loader found for asset type '{}' while loading asset '{}'",
				 AssetFactory::GetAssetTypeInfo( metadata.Type ).Name, NameIfNotNull( metadata.Name ) );

			return nullptr;
		}

		// Create the asset
		asset = loader->Create();
		asset->m_AssetID = metadata.ID;
		asset->m_AssetFlags.SetFlag( EAssetFlags::LoadedFromDisk );

		// We need to fill the load data for the asset loader.
		// If we use asset bundles, we can load the data from there.
		// Otherwise, 
		AssetLoadData loadData{};

	#if USE_ASSET_BUNDLE

		NOT_IMPLEMENTED;

	#else

		// Get the raw asset data from disk
		switch ( metadata.LoadPolicy )
		{
			case EAssetLoadPolicy::Default:
			{
				break;
			}
			default:
			{
				NOT_IMPLEMENTED;
				return nullptr;
			}
		}

	#endif // USE_ASSET_BUNDLE

		Expected<void, String> loadResult = loader->Load( loadData, *asset );
		if ( loadResult.IsError() )
		{
			LOG( LogCategory::Asset, Error, "Failed to load asset '{}' from path '{}': {}",
				 NameIfNotNull( metadata.Name ), metadata.Path, loadResult.Error());
			return nullptr;
		}

		return asset;
	}

	bool AssetDatabase::IsAssetLoaded( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.IsValid() )
			return false;

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it == Get()->m_Assets.end() )
			return false; // Asset does not exist

		IAsset* asset = it->second.second.get();
		return asset && asset->Valid();
	}

	bool AssetDatabase::DoesAssetExist( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.IsValid() )
			return false;

		auto it = Get()->m_Assets.find( a_AssetID );
		return it != Get()->m_Assets.end();
	}

	bool AssetDatabase::RegisterAsset( SharedPtr<IAsset> a_Asset, AssetMetadata a_Metadata )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		if ( a_Asset == nullptr )
			return false;

		const AssetID assetID = a_Asset->ID();
		if ( DoesAssetExist( assetID ) )
			return false; // Asset already exists

		AssetMetadata metadata = a_Metadata;
		metadata.ID = assetID;
		metadata.Type = a_Asset->Type();
		Get()->m_Assets[ assetID ] = { std::move( metadata ), std::move( a_Asset ) };
		return true;
	}

	bool AssetDatabase::UnregisterAsset( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it != Get()->m_Assets.end() )
		{
			Get()->m_AssetPathMap.erase( it->second.first.Path ); // Remove the asset path mapping
			Get()->m_Assets.erase( it );
			return true;
		}
		return false; // Asset was not found
	}

	void AssetDatabase::RegisterDependency( AssetID a_Dependant, AssetID a_Dependency )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		if ( DoesAssetExist( a_Dependant ) == false || DoesAssetExist( a_Dependency ) == false )
			return;

		AssetMetadata& dependantMetadata = Get()->m_Assets[a_Dependant].first;
		dependantMetadata.Dependencies.insert( a_Dependency );
	}

	void AssetDatabase::UnregisterDependency( AssetID a_Dependant, AssetID a_Dependency )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		if ( DoesAssetExist( a_Dependant ) == false || DoesAssetExist( a_Dependency ) == false )
			return;

		AssetMetadata& dependantMetadata = Get()->m_Assets[a_Dependant].first;
		dependantMetadata.Dependencies.erase( a_Dependency );
	}

#if WITH_EDITOR

	AssetID AssetDatabase::ImportAsset( StringView a_Path )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		NOT_IMPLEMENTED;
		return AssetID{};
	}

	bool AssetDatabase::CreateAsset( IAsset* a_Asset, StringView a_Path )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		if (a_Asset == nullptr || a_Path.empty())
			return false; // corrected return type

		SharedPtr<IAsset> asset = a_Asset->Shared();


		NOT_IMPLEMENTED;
		return false;
	}

	bool AssetDatabase::DeleteAsset( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		
		if ( !DoesAssetExist( a_AssetID ) )
			return false;

		// Delete the asset file from disk if it exists.
		StringView assetPath = Get()->GetAssetPathFromID( a_AssetID );
		if ( !assetPath.empty() )
		{
			if ( !IO::DeleteFile( assetPath ) )
			{
				LOG( LogCategory::Asset, Error, "Failed to delete asset file at path '{0}'", assetPath );
				return false;
			}
		}

		UnregisterAsset( a_AssetID ); // Remove the asset from the database
		return true;
	}

#endif

	StringView AssetDatabase::GetAssetName( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		const AssetMetadata* metadata = GetAssetMetadata( a_AssetID );
		if ( metadata && !metadata->Name.empty() )
			return metadata->Name;

		return {};
	}

	Expected<void, String> AssetDatabase::Init()
	{
		if ( s_Instance )
			return Unexpected( "AssetDatabase is already initialized." );

		s_Instance = new AssetDatabase();

		return {};
	}

	Expected<void, String> AssetDatabase::Shutdown()
	{
		if ( !s_Instance )
			return Unexpected( "AssetDatabase is not initialized." );

		if ( s_Instance != this )
			return Unexpected( "AssetDatabase instance mismatch during shutdown." );

		delete s_Instance;
		s_Instance = nullptr;

		return {};
	}

}
