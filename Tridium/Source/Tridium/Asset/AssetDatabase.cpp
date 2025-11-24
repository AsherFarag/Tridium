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

	UUID AssetDatabase::GetAssetIDFromPath( StringView a_Path )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		auto assetID = Get()->m_AssetPathMap.find(a_Path);

		if ( assetID == Get()->m_AssetPathMap.end() )
			return {};

		return assetID->second;
	}

	StringView AssetDatabase::GetAssetPathFromID( UUID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.Valid() )
			return {};

		auto it = Get()->m_Assets.find( a_AssetID );

		if ( it == Get()->m_Assets.end() )
			return {};

		// Gets the path from the asset info
		return it->second.first->Path;
	}

	const AssetInfo* AssetDatabase::GetAssetInfo( UUID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.Valid() )
			return nullptr;

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it == Get()->m_Assets.end() )
			return nullptr;

		return it->second.first.get();
	}

	AssetHandle<IAsset> AssetDatabase::GetAsset( UUID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.Valid() )
			return nullptr;

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it == Get()->m_Assets.end() )
			return nullptr; // Asset does not exist

		return it->second.second;
	}

	AssetHandle<IAsset> AssetDatabase::GetOrLoadAsset( UUID a_AssetID )
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
		SharedPtr<AssetInfo>& assetInfo = assetIt->second.first;
		IAssetLoader* loader = AssetFactory::GetLoader( assetInfo->Type );

		if ( loader == nullptr )
		{
			LOG( LogCategory::Asset, Error, "No loader found for asset type '{}' while loading asset '{}'",
				 AssetFactory::GetAssetTypeInfo( assetInfo->Type ).Name, NameIfNotNull( assetInfo->Name ) );

			return nullptr;
		}

		// Create the asset
		asset = loader->Create();
		asset->m_Info = assetInfo;

		// We need to fill the load data for the asset loader.
		// If we use asset bundles, we can load the data from there.
		// Otherwise, 
		AssetLoadData loadData{};

	#if USE_ASSET_BUNDLE

		NOT_IMPLEMENTED;

	#else

		// Get the raw asset data from disk
		switch ( assetInfo->LoadPolicy )
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
				 NameIfNotNull( assetInfo->Name ), assetInfo->Path, loadResult.Error());
			return nullptr;
		}

		return asset;
	}

	bool AssetDatabase::IsAssetLoaded( UUID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.Valid() )
			return false;

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it == Get()->m_Assets.end() )
			return false; // Asset does not exist

		IAsset* asset = it->second.second.get();
		return asset && asset->Valid();
	}

	bool AssetDatabase::DoesAssetExist( UUID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.Valid() )
			return false;

		auto it = Get()->m_Assets.find( a_AssetID );
		return it != Get()->m_Assets.end();
	}

	bool AssetDatabase::RegisterAsset( SharedPtr<IAsset> a_Asset )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( a_Asset == nullptr )
		{
			CHECK( false, "Cannot register a null asset." );
			return false;
		}

		if ( a_Asset->Info() == nullptr || !a_Asset->Info()->Valid() )
		{
			TODO( "This should be an error" );
			a_Asset->m_Info = MakeShared<AssetInfo>();
			a_Asset->m_Info->ID = UUID::Generate();
			a_Asset->m_Info->Type = a_Asset->Type();
		}

		const UUID assetID = a_Asset->ID();
		if ( DoesAssetExist( assetID ) )
		{
			return false; // Asset already exists
		}

		// Sanitize the asset type
		a_Asset->Type();

		// Map the asset path to the asset ID
		if ( !a_Asset->Info()->Path.empty() )
		{
			Get()->m_AssetPathMap[a_Asset->Info()->Path] = assetID;
		}

		// Map the asset ID to the asset
		Get()->m_Assets[ assetID ] = { a_Asset->Info(), std::move( a_Asset ) };

		return true;
	}

	bool AssetDatabase::UnregisterAsset( UUID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it != Get()->m_Assets.end() )
		{
			Get()->m_AssetPathMap.erase( it->second.first->Path ); // Remove the asset path mapping
			Get()->m_Assets.erase( it );
			return true;
		}
		return false; // Asset was not found
	}

	void AssetDatabase::RegisterDependency( UUID a_Dependant, UUID a_Dependency )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		if ( DoesAssetExist( a_Dependant ) == false || DoesAssetExist( a_Dependency ) == false )
			return;

		AssetInfo* dependantMetadata = Get()->m_Assets[a_Dependant].first.get();
		dependantMetadata->Dependencies.insert( a_Dependency );
	}

	void AssetDatabase::UnregisterDependency( UUID a_Dependant, UUID a_Dependency )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		if ( DoesAssetExist( a_Dependant ) == false || DoesAssetExist( a_Dependency ) == false )
			return;

		AssetInfo* dependantMetadata = Get()->m_Assets[a_Dependant].first.get();
		dependantMetadata->Dependencies.insert( a_Dependency );
	}

#if WITH_EDITOR

	Expected<void, String> AssetDatabase::ImportAsset( const FilePath& a_Path )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		String ext = a_Path.GetExtension().ToString();
		IAssetImporter* importer = AssetFactory::GetImporter( ext );

		if ( importer == nullptr )
		{
			return Unexpected{ std::format( "Unable to find importer for extension {}", ext ) };
		}

		AssetImportContext importContext{ a_Path };

		if ( !importer->OnImport( importContext ) || importContext.ImportFailed() )
		{
			return Unexpected{ importContext.ErrorMessage() };
		}

		if ( !importContext.Warnings().Empty() )
		{
			LOG( LogCategory::Asset, Info, "Encountered warnings while importing '{}'", a_Path.ToString() );

			for ( StringView warning : importContext.Warnings() )
			{
				LOG( LogCategory::Asset, Warn, warning );
			}

			LOG( LogCategory::Asset, Info, "End of asset warning dump" );
		}

		for ( const SharedPtr<IAsset>& createdAsset : importContext.CreatedAssets() )
		{
			if ( !RegisterAsset( createdAsset ) )
			{
				LOG( LogCategory::Asset, Error, "Failed to register imported asset '{}'", NameIfNotNull( createdAsset->Info()->Name ) );
			}
		}

		return {};
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

	bool AssetDatabase::DeleteAsset( UUID a_AssetID )
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

	StringView AssetDatabase::GetAssetName( UUID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		const AssetInfo* metadata = GetAssetInfo( a_AssetID );
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
