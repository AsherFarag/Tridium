#pragma once
#include <Tridium/Asset/AssetImporter.h>

#if USE_ASSET_IMPORTERS

namespace Tridium {

	TODO( "When prefabs are supported, I want this importer to then create a prefab asset that retains the original scene hierarchy." );

	struct ModelImportOptions
	{

	};

	//=============================================================================================
	// Model Importer: Imports 3D model files (e.g. fbx, obj, gltf) and breaks them down into
	// multiple asset types (StaticMesh, Material, Texture, Prefab).
	//=============================================================================================
	DEFINE_ASSET_IMPORTER( ModelImporter )
	{
	public:

		//=============================================================================================
		VersionID Version() const override { return 0; }

		//=============================================================================================
		Array<StringView> SupportedExtensions() const override { return { "fbx", "obj", "gltf" }; }

		//=============================================================================================
		bool OnImport( AssetImportContext& a_Context ) override { return OnImport( a_Context, {} ); }

		//=============================================================================================
		bool OnImport( AssetImportContext& a_Context, const ModelImportOptions& a_Options );

	};

}

#endif