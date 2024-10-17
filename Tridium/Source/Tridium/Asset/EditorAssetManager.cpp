#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorAssetManager.h"
#include <Tridium/Asset/AssetFactory.h>

namespace Tridium::Editor {

	EditorAssetManager::EditorAssetManager()
	{
		Init();
	}

	//////////////////////////////////////////////////////////////////////////
	// Inherited via AssetManagerBase
	//////////////////////////////////////////////////////////////////////////

	void EditorAssetManager::Init()
	{
		AssetFactory::Init();
	}

	void EditorAssetManager::Shutdown()
	{
		if ( SerializeAssetRegistry() )
			TE_CORE_INFO( "[AssetManager] Asset registry serialized successfully" );
		else
			TE_CORE_ERROR( "[AssetManager] Failed to serialize asset registry" );
	}

	SharedPtr<Asset> EditorAssetManager::GetAsset( AssetHandle a_Handle )
    {
		SharedPtr<Asset> asset;

		if ( asset = GetMemoryOnlyAsset( a_Handle ) )
			return asset;

		const AssetMetaData& metaData = GetAssetMetaData( a_Handle );
		// Failed to get meta data
		if ( !metaData.IsValid() )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to get meta data for asset handle: {0}", a_Handle.ID() );
			return nullptr;
		}

		if ( metaData.IsAssetLoaded )
			return m_LoadedAssets[a_Handle];

		// Load the asset
		TE_CORE_INFO( "[AssetManager] Loading asset from: {0}", metaData.Path.ToString() );
		asset = AssetFactory::LoadAsset( metaData );

		if ( !asset )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to load asset from: {0}", metaData.Path.ToString() );
			return nullptr;
		}

		// Asset loaded successfully
		AssetMetaData newMetaData = metaData;
		newMetaData.IsAssetLoaded = true;

		SetAssetMetaData( newMetaData );

		m_LoadedAssets[a_Handle] = asset;
        return asset;
    }

	SharedPtr<Asset> EditorAssetManager::GetMemoryOnlyAsset( AssetHandle a_Handle )
	{
		if ( auto it = m_MemoryAssets.find( a_Handle ); it != m_MemoryAssets.end() )
			return it->second;

		return nullptr;
	}

	bool EditorAssetManager::AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<Asset> a_Asset )
	{
		if ( m_MemoryAssets.contains( a_Handle ) )
		{
			TE_CORE_WARN( "[AssetManager] Memory only asset already exists with handle: {0}", a_Handle.ID() );
			return false;
		}

		m_MemoryAssets[a_Handle] = a_Asset;
		return true;
	}

	bool EditorAssetManager::HasAsset( AssetHandle a_Handle )
	{
		return GetMemoryOnlyAsset( a_Handle ) || GetAssetMetaData( a_Handle ).IsValid();
	}

	void EditorAssetManager::RemoveAsset( AssetHandle a_Handle )
	{
		// Remove memory only asset
		if ( auto it = m_MemoryAssets.find( a_Handle ); it != m_MemoryAssets.end() )
		{
			m_MemoryAssets.erase( it );
			return;
		}

		// Remove loaded asset and meta data

		if ( auto it = m_LoadedAssets.find( a_Handle ); it != m_LoadedAssets.end() )
		{
			m_LoadedAssets.erase( it );
		}

		if ( auto it = m_AssetRegistry.find( a_Handle ); it != m_AssetRegistry.end() )
		{
			m_AssetRegistry.erase( it );
		}
	}

	EAssetType EditorAssetManager::GetAssetType( AssetHandle a_Handle )
	{
		if ( auto memAsset = GetMemoryOnlyAsset( a_Handle ) )
			return memAsset->AssetType();

		auto metaData = GetAssetMetaData( a_Handle );
		if ( metaData.IsValid() )
			return metaData.AssetType;

		return EAssetType::None;
	}

	bool EditorAssetManager::IsMemoryAsset( AssetHandle a_Handle )
	{
		return m_MemoryAssets.contains( a_Handle );
	}

	void EditorAssetManager::RegisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency )
	{
		TE_CORE_INFO( "[AssetManager] Registering dependency: {0} -> {1}", a_Dependent.ID(), a_Dependency.ID() );
		m_AssetDependencies[a_Dependent].insert( a_Dependency );
	}

	void EditorAssetManager::UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency )
	{
		if ( auto it = m_AssetDependencies.find( a_Dependent ); it != m_AssetDependencies.end() )
		{
			TE_CORE_INFO( "[AssetManager] Unregistering dependency: {0} -> {1}", a_Dependent.ID(), a_Dependency.ID() );
			it->second.erase( a_Dependency );
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// Editor Only
	//////////////////////////////////////////////////////////////////////////

	const AssetMetaData& EditorAssetManager::GetAssetMetaData( AssetHandle a_Handle ) const
	{
		if ( auto it = m_AssetRegistry.find( a_Handle ); it != m_AssetRegistry.end() )
			return it->second;

		return AssetMetaData::Invalid;
	}

	const AssetMetaData& EditorAssetManager::GetAssetMetaData( const IO::FilePath& a_Path ) const
	{
		for ( const auto& [handle, metaData] : m_AssetRegistry )
		{
			if ( metaData.Path == a_Path )
				return metaData;
		}

		return AssetMetaData::Invalid;
	}

	void EditorAssetManager::SetAssetMetaData( const AssetMetaData& a_MetaData )
	{
		m_AssetRegistry[a_MetaData.Handle] = a_MetaData;
	}

	AssetHandle EditorAssetManager::ImportAsset( const IO::FilePath& a_Path )
	{
		if ( auto metaData = GetAssetMetaData( a_Path ); metaData.IsValid() )
			return metaData.Handle;

		if ( !a_Path.Exists() )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to import asset: {0}, file does not exist", a_Path.ToString() );
			return AssetHandle::InvalidGUID;
		}

		if ( !a_Path.HasExtension() )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to import asset: {0}, missing file extension", a_Path.ToString() );
			return AssetHandle::InvalidGUID;
		}

		EAssetType assetType = GetAssetTypeFromFileExtension( a_Path.GetExtension() );
		if ( assetType == EAssetType::None )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to import asset: {0}, unsupported file extension", a_Path.ToString() );
			return AssetHandle::InvalidGUID;
		}

		// Create new asset meta data
		AssetMetaData metaData;
		metaData.Handle = AssetHandle::Create();
		metaData.AssetType = assetType;
		metaData.Path = a_Path;
		metaData.IsAssetLoaded = false;

		TE_CORE_INFO( "[AssetManager] Successfully imported asset from: {0}", a_Path.ToString() );
		SetAssetMetaData( metaData );
		return metaData.Handle;
	}

	bool EditorAssetManager::CreateAsset( const AssetMetaData& a_MetaData, SharedPtr<Asset> a_Asset )
	{
		if ( auto it = m_AssetRegistry.find( a_MetaData.Handle ); it != m_AssetRegistry.end() )
		{
			TE_CORE_WARN( "[AssetManager] Asset already exists with handle: {0}", a_MetaData.Handle.ID() );
			return false;
		}

		if ( auto it = m_LoadedAssets.find( a_MetaData.Handle ); it != m_LoadedAssets.end() )
		{
			TE_CORE_WARN( "[AssetManager] Asset already loaded with handle: {0}", a_MetaData.Handle.ID() );
			return false;
		}

		SetAssetMetaData( a_MetaData );
		m_LoadedAssets[a_MetaData.Handle] = a_Asset;
		if ( a_Asset )
			a_Asset->SetHandle( a_MetaData.Handle );

		return true;
	}

	bool EditorAssetManager::SerializeAssetRegistry()
	{
		return false;
	}

	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		return false;
	}
}

#endif