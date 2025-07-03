#include "tripch.h"
#ifdef IS_EDITOR
#include "AssetInfo.h"
#include <Tridium/Engine/Engine.h>
#include <Tridium/oldAsset/Loaders/TextureLoader.h>

namespace Tridium {

	const AssetTypeInfo& AssetTypeManager::GetAssetTypeInfo( EAssetTypeOld type )
	{
		if ( auto it = Get()->m_AssetTypeInfoMap.find(type); it != Get()->m_AssetTypeInfoMap.end() )
		{
			return it->second;
		}

		return Get()->m_AssetTypeInfoMap[EAssetTypeOld::None];
	}

	void AssetTypeManager::Init()
	{
		FilePath iconFolder( Engine::Get()->GetEngineAssetsDirectory() / "Editor/Icons" );
		SharedPtr<TextureOld> defaultIcon = TextureLoader::LoadTexture( iconFolder / "file.png" );

		m_AssetTypeInfoMap[EAssetTypeOld::None] =       { EAssetTypeOld::None,       "None",        Vector4( 0.6f, 0.6f, 0.6f, 1.0f ),  defaultIcon }; // Neutral gray
		m_AssetTypeInfoMap[EAssetTypeOld::Texture] =    { EAssetTypeOld::Texture,    "Texture",     Vector4( 0.2f, 0.6f, 0.8f, 1.0f ),  TextureLoader::LoadTexture( iconFolder / "file-media.png" ) }; // Desaturated cyan
		m_AssetTypeInfoMap[EAssetTypeOld::Material] =   { EAssetTypeOld::Material,   "Material",    Vector4( 0.3f, 0.7f, 0.3f, 1.0f ),  defaultIcon }; // Desaturated green
		m_AssetTypeInfoMap[EAssetTypeOld::MeshSource] = { EAssetTypeOld::MeshSource, "Mesh Source", Vector4( 0.9f, 0.5f, 0.9f, 1.0f ),  defaultIcon }; // Desaturated magenta
		m_AssetTypeInfoMap[EAssetTypeOld::StaticMesh] = { EAssetTypeOld::StaticMesh, "Static Mesh", Vector4( 0.4f, 0.6f, 0.95f, 1.0f ), defaultIcon }; // Desaturated strong blue
		m_AssetTypeInfoMap[EAssetTypeOld::Scene] =      { EAssetTypeOld::Scene,      "Scene",       Vector4( 1.0f, 0.8f, 0.2f, 1.0f ),  TextureLoader::LoadTexture( iconFolder / "tridium-scene.png" ) }; // Warm yellow (unchanged)
		m_AssetTypeInfoMap[EAssetTypeOld::Shader] =     { EAssetTypeOld::Shader,     "Shader",      Vector4( 0.9f, 0.3f, 0.9f, 1.0f ),  defaultIcon }; // Bright pink (unchanged)
		m_AssetTypeInfoMap[EAssetTypeOld::CubeMap] =    { EAssetTypeOld::CubeMap,    "Cube Map",    Vector4( 1.0f, 0.6f, 0.1f, 1.0f ),  TextureLoader::LoadTexture( iconFolder / "file-media.png" ) }; // Vibrant orange (unchanged)
		m_AssetTypeInfoMap[EAssetTypeOld::LuaScript] =  { EAssetTypeOld::LuaScript,  "Lua Script",  Vector4( 0.8f, 0.2f, 0.2f, 1.0f ),  TextureLoader::LoadTexture( iconFolder / "file-code.png" ) }; // Deep red (unchanged)

	}
}

#endif // IS_EDITOR