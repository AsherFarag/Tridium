#include "tripch.h"
#include "AssetDatabase.h"
#include <Tridium/IO/FileIO.h>

namespace Tridium {

	using namespace T;

	AssetDatabase* AssetDatabase::s_Instance = nullptr;

	inline constexpr StringView NameIfNotNull( StringView a_Name )
	{
		return a_Name.empty() ? "<UNNAMED>" : a_Name;
	}

	AssetID AssetDatabase::GetAssetIDFromPath( StringView a_Path )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		auto assetID = Get()->m_AssetPathMap.find( a_Path );
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

	IAsset* AssetDatabase::GetAsset( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		if ( !a_AssetID.IsValid() )
			return nullptr;

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it == Get()->m_Assets.end() )
			return nullptr; // Asset does not exist

		return it->second.second.get();
	}

	IAsset* AssetDatabase::GetOrLoadAsset( AssetID a_AssetID )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );

		auto it = Get()->m_Assets.find( a_AssetID );
		if ( it == Get()->m_Assets.end() )
			return nullptr; // Asset does not exist

		IAsset* assetPtr = it->second.second.get();
		if ( assetPtr )
			return assetPtr; // Asset is already loaded

		// The asset exists but is not loaded, attempt to load it

	#if WITH_EDITOR

		AssetMetadata& metadata = it->second.first;
		IAssetLoader* loader = Get()->m_AssetFactory.GetLoader( metadata.Type );
		if ( !loader )
		{
			LOG( LogCategory::Asset, Error, "No loader found for asset type '{0}' while loading asset '{1}'",
				ToString( metadata.Type ), NameIfNotNull( metadata.Name ) );
			return nullptr;
		}

		SharedPtr<IAsset> asset{};

		// Load the asset depending on its load policy
		switch ( metadata.LoadPolicy )
		{
		case EAssetLoadPolicy::Default:
		{
			asset = loader->CreateAsset( metadata.ID, EAssetFlags::None );
			asset->m_AssetFlags.RemoveFlag( EAssetFlags::MemoryOnly ); // Ensure it's not memory-only

			Expected<void, String> loadResult = loader->Load( asset, metadata );
			if ( loadResult.IsError() )
			{
				LOG( LogCategory::Asset, Error, "Failed to load asset '{0}' from path '{1}': {2}",
					NameIfNotNull( metadata.Name ), metadata.Path, loadResult.Error() );
				return nullptr;
			}
		}
		break;
		default:
			NOT_IMPLEMENTED;
			return nullptr;
		}

		assetPtr = asset.get();
		it->second.second = std::move( asset ); // Store the loaded asset
		return assetPtr;

	#else
		NOT_IMPLEMENTED;
	#endif
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

	bool AssetDatabase::RegisterAsset( const SharedPtr<IAsset>& a_Asset, const AssetMetadata& a_Metadata )
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
		Get()->m_Assets[assetID] = { std::move( metadata ), a_Asset };
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

	AssetID AssetDatabase::Editor::ImportAsset( StringView a_Path )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		NOT_IMPLEMENTED;
		return AssetID{};
	}

	bool AssetDatabase::Editor::CreateAsset( IAsset* a_Asset, StringView a_Path )
	{
		CHECK( s_Instance, "AssetDatabase is not initialized." );
		if (a_Asset == nullptr || a_Path.empty())
			return false; // corrected return type

		SharedPtr<IAsset> asset = a_Asset->Shared();


		NOT_IMPLEMENTED;
		return false;
	}

	bool AssetDatabase::Editor::DeleteAsset( AssetID a_AssetID )
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

		return "<UNNAMED>";
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

		delete s_Instance;
		s_Instance = nullptr;

		return {};
	}

}
