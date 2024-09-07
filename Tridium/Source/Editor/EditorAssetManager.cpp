#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorAssetManager.h"
#include <Tridium/Asset/Asset.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/Loaders/AssetLoader.h>
#include <Tridium/Asset/AssetMetaData.h>
#include <Tridium/Asset/AssetFileExtensions.h>
#include "Editor.h"

namespace Tridium::Editor {

    bool EditorAssetManager::HasAssetImpl( const AssetHandle& a_AssetHandle ) const
    {
        return m_Library.find(a_AssetHandle) != m_Library.end();
    }

    AssetRef<Asset> EditorAssetManager::GetAssetImpl( const AssetHandle& a_AssetHandle )
    {
        // Check if the asset is already loaded
	    if ( auto it = m_Library.find(a_AssetHandle); it != m_Library.end() )
	    	return it->second;
    
	    return nullptr;
    }

    AssetRef<Asset> EditorAssetManager::GetAssetImpl( const IO::FilePath& a_Path )
    {
        AssetHandle assetHandle = GetAssetHandleImpl(a_Path);

        // Check if the asset is already loaded
        if ( auto it = m_Library.find( assetHandle ); it != m_Library.end() )
            return it->second;

        return nullptr;
    }

    AssetRef<Asset> EditorAssetManager::LoadAssetImpl( const AssetHandle& a_AssetHandle )
    {
        auto it = m_Paths.find( a_AssetHandle );
        if ( it == m_Paths.end() )
            return nullptr;

        return LoadAssetImpl( it->second );
    }

    AssetRef<Asset> EditorAssetManager::LoadAssetImpl( const IO::FilePath& a_Path )
    {  
        Scope<AssetMetaData> metaData( LoadAssetMetaData( a_Path + IO::MetaExtension ) );
        // If this asset file has no meta data file, we must import the asset file
        if ( !metaData )
            return ImportAssetImpl( a_Path, true );

        const IAssetLoaderInterface* assetLoader = AssetFactory::GetLoader( metaData->AssetType );
        // If there is no loader for this asset type
        if ( !assetLoader )
            return nullptr;

        AssetRef<Asset> asset = assetLoader->DebugLoad( a_Path, metaData.Get() );

        if ( !asset )
            return nullptr;

        ApplyMetaDataToAsset( *asset, *metaData );

        return AddAssetImpl( asset ) ? asset : nullptr;
    }

    bool EditorAssetManager::ReleaseAssetImpl( const AssetHandle& a_AssetHandle )
    {
        auto it = m_Library.find( a_AssetHandle );
        if ( it == m_Library.end() )
            return false;

        m_Library.erase( it );
        return true;
    }

    void EditorAssetManager::ApplyMetaDataToAsset( Asset& a_Asset, const AssetMetaData& a_MetaData )
    {
        a_Asset.m_Handle = a_MetaData.Handle;
    }

    AssetRef<Asset> EditorAssetManager::ImportAssetImpl( const IO::FilePath& a_Path, bool a_Override )
    {
        if ( !a_Override && LoadAssetMetaDataImpl( a_Path + IO::MetaExtension ) )
            return LoadAsset( a_Path );

        EAssetType assetType = IO::GetAssetTypeFromExtension( a_Path.GetExtension() );
        Scope<AssetMetaData> metaData = AssetFactory::ConstructAssetMetaData( assetType );
        if ( !metaData )
            return nullptr;

    }

    bool EditorAssetManager::SaveAssetImpl( const IO::FilePath& a_Path, const AssetRef<Asset>& a_Asset )
    {
        if ( !a_Asset )
            return false;

        return AssetFactory::SaveAsset( a_Path, a_Asset );
    }


    bool EditorAssetManager::AddAssetImpl( const AssetRef<Asset>& a_Asset )
    {
        if ( !a_Asset || !a_Asset.GetAssetHandle().Valid() )
            return false;

        if ( HasAssetImpl( a_Asset.GetAssetHandle() ) )
            return false;

        m_Library.insert( { a_Asset.GetAssetHandle(), a_Asset } );
        m_Paths.insert( { a_Asset.GetAssetHandle(), a_Asset->GetPath() } );

        return true;
    }

    AssetHandle EditorAssetManager::GetAssetHandleImpl( const IO::FilePath& a_Path ) const
    {
        TODO( "Slow! Cache Paths-to-Handles or something." );

        for ( auto& [handle, path] : m_Paths )
        {
            if ( path == a_Path )
                return handle;
        }

        return {};
    }

    AssetMetaData* EditorAssetManager::LoadAssetMetaDataImpl( const IO::FilePath& a_Path ) const
    {
        return nullptr;
    }

}

#endif