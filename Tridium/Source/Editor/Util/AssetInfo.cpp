#include "tripch.h"
#ifdef IS_EDITOR
#include "AssetInfo.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>

namespace Tridium
{
	namespace Editor
	{
		UniquePtr<AssetTypeManager> AssetTypeManager::s_Instance = nullptr;

		const AssetTypeInfo& AssetTypeManager::GetAssetTypeInfo( EAssetType type )
		{
			if ( auto it = s_Instance->m_AssetTypeInfoMap.find( type ); it != s_Instance->m_AssetTypeInfoMap.end() )
			{
				return it->second;
			}

			return s_Instance->m_AssetTypeInfoMap[EAssetType::None];
		}

		void AssetTypeManager::Initialize()
		{
			IO::FilePath iconFolder( Application::GetEngineAssetsDirectory() / "Editor/Icons" );
			SharedPtr<Texture> defaultIcon = TextureLoader::LoadTexture( iconFolder / "file.png" );

			m_AssetTypeInfoMap[EAssetType::None]          = { EAssetType::None,       "None",        Vector4( 0.6f, 0.6f, 0.6f, 1.0f ), defaultIcon }; // Neutral gray
			m_AssetTypeInfoMap[EAssetType::Texture]       = { EAssetType::Texture,    "Texture",     Vector4( 0.2f, 0.6f, 0.8f, 1.0f ), TextureLoader::LoadTexture( iconFolder / "file-media.png" ) }; // Desaturated cyan
			m_AssetTypeInfoMap[EAssetType::Material]      = { EAssetType::Material,   "Material",    Vector4( 0.3f, 0.7f, 0.3f, 1.0f ), defaultIcon }; // Desaturated green
			m_AssetTypeInfoMap[EAssetType::MeshSource]    = { EAssetType::MeshSource, "Mesh Source", Vector4( 0.9f, 0.5f, 0.9f, 1.0f ), defaultIcon }; // Desaturated magenta
			m_AssetTypeInfoMap[EAssetType::StaticMesh]    = { EAssetType::StaticMesh, "Static Mesh", Vector4( 0.4f, 0.6f, 0.95f, 1.0f ), defaultIcon }; // Desaturated strong blue
			m_AssetTypeInfoMap[EAssetType::Scene]         = { EAssetType::Scene,      "Scene",       Vector4( 1.0f, 0.8f, 0.2f, 1.0f ), TextureLoader::LoadTexture( iconFolder / "tridium-scene.png" ) }; // Warm yellow (unchanged)
			m_AssetTypeInfoMap[EAssetType::Shader]        = { EAssetType::Shader,     "Shader",      Vector4( 0.9f, 0.3f, 0.9f, 1.0f ), defaultIcon }; // Bright pink (unchanged)
			m_AssetTypeInfoMap[EAssetType::CubeMap]       = { EAssetType::CubeMap,    "Cube Map",    Vector4( 1.0f, 0.6f, 0.1f, 1.0f ), TextureLoader::LoadTexture( iconFolder / "file-media.png" ) }; // Vibrant orange (unchanged)
			m_AssetTypeInfoMap[EAssetType::Lua]           = { EAssetType::Lua,        "Lua",         Vector4( 0.8f, 0.2f, 0.2f, 1.0f ), TextureLoader::LoadTexture( iconFolder / "file-code.png" ) }; // Deep red (unchanged)

		}
	}
}

#endif // IS_EDITOR