#include "tripch.h"
#include "AssetType.h"

namespace Tridium {

    EAssetTypeOld GetAssetTypeFromFileExtension( const std::string& a_Extension )
    {
		static const std::unordered_map<std::string, EAssetTypeOld> s_Extensions =
		{
			// Tridium
			{ ".tscene", EAssetTypeOld::Scene },
			{ ".tmat", EAssetTypeOld::Material },
			{ ".tmesh", EAssetTypeOld::StaticMesh },

			// Mesh Sources
			{ ".obj", EAssetTypeOld::MeshSource },
			{ ".fbx", EAssetTypeOld::MeshSource },
			{ ".gltf", EAssetTypeOld::MeshSource },

			// Rendering
			{ ".glsl", EAssetTypeOld::Shader },

			// Textures
			{ ".jpg", EAssetTypeOld::Texture },
			{ ".png", EAssetTypeOld::Texture },
			{ ".tga", EAssetTypeOld::Texture },
			{ ".jpeg", EAssetTypeOld::Texture },

			// Cube Maps
			{ ".hdr", EAssetTypeOld::CubeMap },

			// Lua
			{ ".lua", EAssetTypeOld::LuaScript },
		};

		if ( auto it = s_Extensions.find( Util::ToLower( StringView{ a_Extension } ) ); it != s_Extensions.end() )
		{
			return it->second;
		}

		return EAssetTypeOld::None;
    }

}