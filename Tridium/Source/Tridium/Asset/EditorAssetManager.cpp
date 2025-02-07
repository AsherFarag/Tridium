#include "tripch.h"
#if 1// IS_EDITOR
#include "EditorAssetManager.h"
#include <Tridium/Asset/AssetFactory.h>
#include <yaml-cpp/yaml.h>
#include <fstream>

#include <Tridium/Graphics/Rendering/Mesh.h>
#include <Tridium/Graphics/Rendering/Texture.h>

namespace Tridium::Editor {

	EditorAssetManager::EditorAssetManager()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	// Inherited via AssetManagerBase
	//////////////////////////////////////////////////////////////////////////

	void EditorAssetManager::Init()
	{
		AssetFactory::Init();
		DeserializeAssetRegistry();

		// Initialize asset factories
		MeshFactory::Init();
		TextureFactory::Init();
	}

	void EditorAssetManager::Shutdown()
	{
		//for ( auto& [handle, asset] : m_LoadedAssets )
		//{
		//	AssetFactory::SaveAsset( GetAssetMetaData( handle ), asset );
		//}

		//SerializeAssetRegistry();
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
			//TE_CORE_ERROR( "[AssetManager] Failed to get meta data for asset handle: {0}", a_Handle.ID() );
			return nullptr;
		}

		if ( metaData.IsAssetLoaded )
			return m_LoadedAssets[a_Handle];

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// The asset needs to be loaded
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// Load the dependencies first
		for ( const auto& dependency : m_AssetRegistry.AssetDependencies[a_Handle] )
		{
			if ( !dependency.IsValid() )
				continue;

			if ( !GetAsset( dependency ) )
			{
				TE_CORE_ERROR( "[AssetManager] Failed to load dependency: {0} for asset: {1}", dependency.ID(), a_Handle.ID() );
				return nullptr;
			}
		}

		// Load the asset
		AssetMetaData newMetaData =
		{
			.Handle = metaData.Handle,
			.AssetType = metaData.AssetType,
			.Path = GetAbsolutePath( metaData.Path ),
			.Name = metaData.Name,
			.IsAssetLoaded = false
		};

		TE_CORE_INFO( "[AssetManager] Loading asset from: {0}", newMetaData.Path.ToString() );
		asset = AssetFactory::LoadAsset( newMetaData );

		if ( !asset )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to load asset from: {0}", metaData.Path.ToString() );
			return nullptr;
		}

		// Asset loaded successfully
		newMetaData.IsAssetLoaded = true;
		TODO( "Dirty way of converting a once relative path back into a relative path." );
		newMetaData.Path = metaData.Path;

		SetAssetMetaData( newMetaData );

		m_LoadedAssets[a_Handle] = asset;
        return asset;
    }

	SharedPtr<Asset> EditorAssetManager::GetAsset( const IO::FilePath& a_Path )
	{
		const AssetMetaData& metaData = GetAssetMetaData( a_Path );
		if ( !metaData.IsValid() )
			return nullptr;

		return GetAsset( metaData.Handle );
	}

	SharedPtr<Asset> EditorAssetManager::GetMemoryOnlyAsset( AssetHandle a_Handle )
	{
		if ( auto it = m_MemoryAssets.find( a_Handle ); it != m_MemoryAssets.end() )
			return it->second;

		return nullptr;
	}

	AssetStorageIterator EditorAssetManager::GetAssets()
	{
		return AssetStorageIterator( m_LoadedAssets, m_MemoryAssets );
	}

	bool EditorAssetManager::AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<Asset> a_Asset )
	{
		TE_CORE_ASSERT( a_Asset, "[AssetManager] Asset is nullptr" );

		if ( m_MemoryAssets.contains( a_Handle ) )
		{
			TE_CORE_WARN( "[AssetManager] Memory only asset already exists with handle: {0}", a_Handle.ID() );
			return false;
		}

		m_MemoryAssets[a_Handle] = a_Asset;
		a_Asset->SetHandle( a_Handle );

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

		if ( auto it = m_AssetRegistry.AssetMetaData.find( a_Handle ); it != m_AssetRegistry.AssetMetaData.end() )
		{
			( *it ).second.IsAssetLoaded = false;
		}

		// Remove dependencies
		for ( auto& [dependent, dependencies] : m_AssetRegistry.AssetDependencies )
		{
			dependencies.erase( a_Handle );
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
		//TE_CORE_INFO( "[AssetManager] Registering dependency: {0} -> {1}", a_Dependent.ID(), a_Dependency.ID() );
		m_AssetRegistry.AssetDependencies[a_Dependent].insert( a_Dependency );
	}

	void EditorAssetManager::UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency )
	{
		if ( auto it = m_AssetRegistry.AssetDependencies.find( a_Dependent ); it != m_AssetRegistry.AssetDependencies.end() )
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
		if ( auto it = m_AssetRegistry.AssetMetaData.find( a_Handle ); it != m_AssetRegistry.AssetMetaData.end() )
			return it->second;

		return AssetMetaData::s_InvalidMetaData;
	}

	//////////////////////////////////////////////////////////////////////////
	const AssetMetaData& EditorAssetManager::GetAssetMetaData( const IO::FilePath& a_Path ) const
	{
		TODO( "Incredibly inefficient, fix this" );
		const IO::FilePath& path = GetAbsolutePath( a_Path );
		for ( const auto& [handle, metaData] : m_AssetRegistry.AssetMetaData )
		{
			if ( GetAbsolutePath( metaData.Path ) == path )
				return metaData;
		}

		return AssetMetaData::s_InvalidMetaData;
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorAssetManager::SetAssetMetaData( const AssetMetaData& a_MetaData )
	{
		m_AssetRegistry.AssetMetaData[a_MetaData.Handle] = a_MetaData;
	}

	//////////////////////////////////////////////////////////////////////////
	bool EditorAssetManager::SaveAsset( AssetHandle a_Handle )
	{
		const AssetMetaData& metaData = GetAssetMetaData( a_Handle );
		if ( !metaData.IsValid() )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to save asset: {0}, meta data not found", a_Handle.ID() );
			return false;
		}

		// The asset is not loaded, so we can't save it
		if ( !metaData.IsAssetLoaded )
			return true;

		if ( auto asset = GetAsset( a_Handle ); asset )
		{
			AssetFactory::SaveAsset( metaData, asset );
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	AssetHandle EditorAssetManager::ImportAsset( const IO::FilePath& a_Path )
	{
		if ( auto metaData = GetAssetMetaData( a_Path ); metaData.IsValid() )
			return metaData.Handle;

		IO::FilePath absolutePath = GetAbsolutePath( a_Path );

		if ( !absolutePath.Exists() )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to import asset: {0}, file does not exist", a_Path.ToString() );
			return AssetHandle::InvalidID;
		}

		if ( !absolutePath.HasExtension() )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to import asset: {0}, missing file extension", a_Path.ToString() );
			return AssetHandle::InvalidID;
		}

		EAssetType assetType = GetAssetTypeFromFileExtension( a_Path.GetExtension() );
		if ( assetType == EAssetType::None )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to import asset: {0}, unsupported file extension", a_Path.ToString() );
			return AssetHandle::InvalidID;
		}

		// Create new asset meta data
		AssetMetaData metaData;
		metaData.Handle = AssetHandle::Create();
		metaData.AssetType = assetType;
		metaData.Path = a_Path;
		metaData.Name = a_Path.GetFilenameWithoutExtension();
		metaData.IsAssetLoaded = false;

		TE_CORE_INFO( "[AssetManager] Successfully imported asset from: {0}", a_Path.ToString() );
		SetAssetMetaData( metaData );
		return metaData.Handle;
	}

	//////////////////////////////////////////////////////////////////////////
	bool EditorAssetManager::CreateAsset( const AssetMetaData& a_MetaData, SharedPtr<Asset> a_Asset )
	{
		if ( m_AssetRegistry.AssetMetaData.find( a_MetaData.Handle ) != m_AssetRegistry.AssetMetaData.end() )
		{
			TE_CORE_WARN( "[AssetManager] Asset already exists with handle: {0}", a_MetaData.Handle.ID() );
			return false;
		}

		if ( m_LoadedAssets.find( a_MetaData.Handle ) != m_LoadedAssets.end() )
		{
			TE_CORE_WARN( "[AssetManager] Asset already loaded with handle: {0}", a_MetaData.Handle.ID() );
			return false;
		}

		SetAssetMetaData( a_MetaData );
		m_LoadedAssets[a_MetaData.Handle] = a_Asset;
		if ( a_Asset )
			a_Asset->SetHandle( a_MetaData.Handle );

		SaveAsset( a_MetaData.Handle );

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	IO::FilePath EditorAssetManager::GetAbsolutePath( const IO::FilePath& a_Path ) const
	{
		// If the path is already absolute, return it
		if ( a_Path.IsAbsolute() )
			return a_Path;

		// Else, the path is relative to the project directory
		return Application::GetActiveProject()->GetConfiguration().ProjectDirectory / a_Path;
	}

	//////////////////////////////////////////////////////////////////////////
	bool EditorAssetManager::SerializeAssetRegistry()
	{
		TE_CORE_INFO( "[AssetManager] Serializing asset registry" );
		// Create a timer
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "AssetMetaData";
		out << YAML::Value << YAML::BeginSeq; // AssetMetaData

		for ( const auto& [handle, metaData] : m_AssetRegistry.AssetMetaData )
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Handle" << YAML::Value << handle.ID();
			out << YAML::Key << "AssetType" << YAML::Value << AssetTypeToString( metaData.AssetType );
			out << YAML::Key << "Path" << YAML::Value << metaData.Path.ToString();
			out << YAML::Key << "Name" << YAML::Value << metaData.Name;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq; // AssetMetaData

		out << YAML::Key << "AssetDependencies";
		out << YAML::Value << YAML::BeginMap; // AssetDependencies

		for ( const auto& [dependent, dependencies] : m_AssetRegistry.AssetDependencies )
		{
			out << YAML::Key << dependent.ID();
			out << YAML::Value << YAML::BeginSeq; // Dependencies

			for ( const auto& dependency : dependencies )
			{
				out << dependency.ID();
			}

			out << YAML::EndSeq; // Dependencies
		}


		TODO( "Implement a proper path system for the asset registry" );
		const IO::FilePath registryPath = Application::GetActiveProject()->GetConfiguration().ProjectDirectory / "TridiumAssetRegistry.yaml";

		std::ofstream file( registryPath.ToString() );
		file << out.c_str();
		file.close();

		TE_CORE_INFO( "[AssetManager] Asset registry serialized in: {0}ms", 
			std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - begin ).count() );

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		YAML::Node data;
		try
		{
			TODO( "Implement a proper path system for the asset registry" );
			const IO::FilePath registryPath =  Application::GetActiveProject()->GetConfiguration().ProjectDirectory / "TridiumAssetRegistry.yaml";
			data = YAML::LoadFile( registryPath.ToString() );
		}
		catch ( const YAML::BadFile& e )
		{
			TE_CORE_ERROR( "[AssetManager] Failed to deserialize asset registry: {0}", e.what() );
			return false;
		}

		// Deserialize AssetMetaData
		if ( auto assetMetaData = data["AssetMetaData"]; assetMetaData )
		{
			for ( const auto& asset : assetMetaData )
			{
				AssetMetaData metaData;
				metaData.Path = asset["Path"].as<std::string>();

				// Strip absolute path if it is inside the content directory
				if ( metaData.Path.IsAbsolute() )
				{
					metaData.Path = IO::FilePath::Relative( metaData.Path, Application::GetActiveProject()->GetConfiguration().ProjectDirectory );
				}

				if ( !GetAbsolutePath( metaData.Path ).Exists() )
				{
					TE_CORE_WARN( "[AssetManager] Failed to deserialize imported asset: {0} does not exist. - Removing from asset registry!", metaData.Path.ToString() );
					continue;
				}

				metaData.Handle = asset["Handle"].as<uint64_t>();
				metaData.AssetType = AssetTypeFromString( asset["AssetType"].as<std::string>().c_str() );
				metaData.Name = asset["Name"].as<std::string>();

				if ( metaData.Name.empty() )
					metaData.Name = metaData.Path.GetFilenameWithoutExtension();

				SetAssetMetaData( metaData );
			}
		}
		else
		{
			TE_CORE_ERROR( "[AssetManager] Failed to deserialize asset registry: missing AssetMetaData" );
			return false;
		}

		// Deserialize AssetDependencies
		if ( auto assetDependencies = data["AssetDependencies"]; assetDependencies )
		{
			for ( const auto& assetDependency : assetDependencies )
			{
				AssetHandle dependent = assetDependency.first.as<uint64_t>();
				for ( const auto& dependency : assetDependency.second )
				{
					RegisterDependency( dependent, dependency.as<uint64_t>() );
				}
			}
		}
		else
		{
			TE_CORE_ERROR( "[AssetManager] Failed to deserialize asset registry: missing AssetDependencies" );
			return false;
		}

		return true;
	}
}

#endif