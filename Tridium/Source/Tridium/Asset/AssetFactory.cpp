#include "tripch.h"
#include "AssetFactory.h"
#include "Asset.h"
#include "Loaders/AssetLoader.h"
#include <Tridium/Asset/AssetFileExtensions.h>
#include <Tridium/IO/FilePath.h>
#include <Tridium/IO/Archive.h>

#include <Tridium/Asset/Loaders/TextureLoader.h>
#include <Tridium/Asset/Loaders/ShaderLoader.h>
#include <Tridium/Asset/Loaders/MaterialLoader.h>
#include <Tridium/Asset/Loaders/ModelLoader.h>

namespace Tridium {

	void AssetFactory::InitializeLoaders()
	{
		m_Loaders[EAssetType::Texture] = new TextureLoader();
		m_Loaders[EAssetType::Shader] = new ShaderLoader();
		m_Loaders[EAssetType::Material] = new MaterialLoader();
		m_Loaders[EAssetType::Mesh] = new ModelLoader();
	}

	void AssetFactory::RegisterLoader( EAssetType a_AssetType, IAssetLoader& a_AssetLoader )
	{
		if ( m_Loaders.find( a_AssetType ) != m_Loaders.end() )
			return;

		m_Loaders[a_AssetType] = &a_AssetLoader;
	}

	IAssetLoader* AssetFactory::GetLoader( EAssetType a_AssetType )
	{
		auto it = m_Loaders.find( a_AssetType );
		return it != m_Loaders.end() ? it->second : nullptr;
	}

	AssetRef<Asset> AssetFactory::LoadAsset( EAssetType a_AssetType, const IO::FilePath& a_Path )
	{
		IAssetLoader* loader = GetLoader( a_AssetType );
		return loader ? loader->Load( a_Path ) : nullptr;
	}

	AssetMetaData* AssetFactory::LoadAssetMetaData( EAssetType a_AssetType, const YAML::Node& a_Node )
	{
		IAssetLoader* loader = GetLoader( a_AssetType );
		return loader ? loader->LoadAssetMetaData( a_Node ) : nullptr;
	}

	AssetMetaData* AssetFactory::ConstructAssetMetaData( EAssetType a_AssetType )
	{
		IAssetLoader* loader = GetLoader( a_AssetType );
		return loader ? loader->ConstructAssetMetaData() : nullptr;
	}

	bool AssetFactory::SaveAsset( const IO::FilePath& a_Path, AssetRef<Asset> a_Asset )
	{
		IAssetLoader* loader = GetLoader( a_Asset->AssetType() );
		return loader ? loader->Save( a_Path, a_Asset.Get() ) : false;
	}
}