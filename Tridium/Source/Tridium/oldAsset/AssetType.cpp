#include "tripch.h"
#include "AssetType.h"

namespace Tridium {

    EAssetType GetAssetTypeFromFileExtension( const std::string& a_Extension )
    {
		static const std::unordered_map<std::string, EAssetType> s_Extensions =
		{
			// Tridium
			{ ".tscene", EAssetType::Scene },
			{ ".tmat", EAssetType::Material },
			{ ".tmesh", EAssetType::StaticMesh },

			// Mesh Sources
			{ ".obj", EAssetType::MeshSource },
			{ ".fbx", EAssetType::MeshSource },
			{ ".gltf", EAssetType::MeshSource },

			// Rendering
			{ ".glsl", EAssetType::Shader },

			// Textures
			{ ".jpg", EAssetType::Texture },
			{ ".png", EAssetType::Texture },
			{ ".tga", EAssetType::Texture },
			{ ".jpeg", EAssetType::Texture },

			// Cube Maps
			{ ".hdr", EAssetType::CubeMap },

			// Lua
			{ ".lua", EAssetType::LuaScript },
		};

		if ( auto it = s_Extensions.find( Util::ToLower( StringView{ a_Extension } ) ); it != s_Extensions.end() )
		{
			return it->second;
		}

		return EAssetType::None;
    }

}