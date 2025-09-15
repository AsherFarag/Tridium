#pragma once
#include <Tridium/Asset/AssetImporter.h>

#if USE_ASSET_IMPORTERS

namespace Tridium {

	TODO( "When prefabs are supported, I want this importer to then create a prefab asset that retains the original scene hierarchy." );

	DEFINE_ASSET_IMPORTER( ModelImporter )
	{
	public:

		VersionID Version() const override { return 0; }

		Array<StringView> SupportedExtensions() const override { return { "fbx", "obj", "gltf" }; }

		bool OnImport( AssetImportContext& a_Context ) override;

	};

}

#endif