#pragma once
#include <unordered_set>
#include <string>
#include "AssetType.h"

namespace Tridium {
	namespace IO {

		using FileExtension = std::string;
		using FileExtensionSet = std::unordered_set<FileExtension>;

		// ----- Tridium File Extensions -----

		const FileExtension ProjectExtension = ".tproject";
		const FileExtension SceneExtension = ".tscene";
		const FileExtension MaterialExtension = ".tmat";
		const FileExtension MetaExtension = ".tmeta";

		// -----------------------------------

		const FileExtension LUAExtension = ".lua";

		const FileExtension FBXExtension = ".fbx";

		const FileExtensionSet ShaderExtensions = { ".glsl" };
		const FileExtensionSet TextureExtensions = { ".png", ".tga", ".jpeg" };
		const FileExtensionSet ModelExtensions = { ".obj" };

		constexpr EAssetType GetAssetTypeFromExtension( const std::string& a_FileExtension )
		{
			if ( a_FileExtension.empty() )
				return EAssetType::Folder;

			if ( a_FileExtension == LUAExtension )
				return EAssetType::Lua;

			if ( a_FileExtension == ProjectExtension )
				return EAssetType::Project;

			if ( a_FileExtension == SceneExtension )
				return EAssetType::Scene;

			if ( a_FileExtension == MaterialExtension )
				return EAssetType::Material;

			if ( ShaderExtensions.find( a_FileExtension ) != ShaderExtensions.end() )
				return EAssetType::Shader;

			if ( TextureExtensions.find( a_FileExtension ) != TextureExtensions.end() )
				return EAssetType::Texture;

			if ( ModelExtensions.find( a_FileExtension ) != ModelExtensions.end() )
				return EAssetType::Mesh;

			return EAssetType::None;
		}

	}
}