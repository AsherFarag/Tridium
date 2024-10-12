#pragma once
#include <Tridium/Core/Types.h>

namespace Tridium {

    using AssetHandle = GUID;

    enum class EAssetType : Byte
    {
        None = 0,
        // Rendering
        Mesh, Shader, Texture, Material,
        // Misc
        Folder,
        // Scripting
        Lua,
        // Tridium
        Project, Scene
    };

	static const char* AssetTypeToString( EAssetType a_Type )
	{
		switch ( a_Type )
		{
		case EAssetType::Mesh: return "Mesh";
		case EAssetType::Shader: return "Shader";
		case EAssetType::Texture: return "Texture";
		case EAssetType::Material: return "Material";
		case EAssetType::Folder: return "Folder";
		case EAssetType::Lua: return "Lua";
		case EAssetType::Project: return "Project";
		case EAssetType::Scene: return "Scene";
		}

		return "None";
	}

	static EAssetType AssetTypeFromString( const char* a_Type )
	{
		if ( strcmp( a_Type, "Mesh" ) == 0 ) return EAssetType::Mesh;
		if ( strcmp( a_Type, "Shader" ) == 0 ) return EAssetType::Shader;
		if ( strcmp( a_Type, "Texture" ) == 0 ) return EAssetType::Texture;
		if ( strcmp( a_Type, "Material" ) == 0 ) return EAssetType::Material;
		if ( strcmp( a_Type, "Folder" ) == 0 ) return EAssetType::Folder;
		if ( strcmp( a_Type, "Lua" ) == 0 ) return EAssetType::Lua;
		if ( strcmp( a_Type, "Project" ) == 0 ) return EAssetType::Project;
		if ( strcmp( a_Type, "Scene" ) == 0 ) return EAssetType::Scene;

		return EAssetType::None;
	}


}