#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/GUID.h>
#include <Tridium/Utils/StringHelpers.h>

namespace Tridium {

    using AssetHandle = GUID;

    enum class EAssetType : Byte
    {
        None = 0,
		// Tridium 
		StaticMesh, Scene, Material,

		// Rendering
		MeshSource, Shader, Texture, 

        // Scripting
        Lua,
    };

	static const char* AssetTypeToString( EAssetType a_Type )
	{
		switch ( a_Type )
		{
		case EAssetType::StaticMesh: return "Mesh";
		case EAssetType::Scene: return "Scene";
		case EAssetType::Material: return "Material";
		case EAssetType::MeshSource: return "MeshSource";
		case EAssetType::Shader: return "Shader";
		case EAssetType::Texture: return "Texture";
		case EAssetType::Lua: return "Lua";
		}

		return "None";
	}

	static EAssetType AssetTypeFromString( const char* a_Type )
	{
		if ( strcmp( a_Type, "Mesh" ) == 0 ) return EAssetType::StaticMesh;
		if ( strcmp( a_Type, "MeshSource" ) == 0 ) return EAssetType::MeshSource;
		if ( strcmp( a_Type, "Shader" ) == 0 ) return EAssetType::Shader;
		if ( strcmp( a_Type, "Texture" ) == 0 ) return EAssetType::Texture;
		if ( strcmp( a_Type, "Material" ) == 0 ) return EAssetType::Material;
		if ( strcmp( a_Type, "Lua" ) == 0 ) return EAssetType::Lua;
		if ( strcmp( a_Type, "Scene" ) == 0 ) return EAssetType::Scene;

		return EAssetType::None;
	}

	EAssetType GetAssetTypeFromFileExtension( const std::string& a_Extension );
}