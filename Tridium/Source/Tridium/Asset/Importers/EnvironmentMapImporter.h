#pragma once
#include <Tridium/Asset/AssetImporter.h>

#if USE_ASSET_IMPORTERS

#include <Tridium/Asset/EnvironmentMapAsset.h>

namespace Tridium {

	//=============================================================================================
	// Environment Map Importer: Imports equirectangular environment maps (hdr, png, jpg) and
	// converts them into cubemaps for use in the engine.
	//=============================================================================================
	DEFINE_ASSET_IMPORTER( EnvironmentMapImporter )
	{
	public:

		//=============================================================================================
		VersionID Version() const override { return 0; }

		//=============================================================================================
		Array<StringView> SupportedExtensions() const override { return { "hdr"/*, "png", "jpg", "jpeg"*/ }; }

		//=============================================================================================
		bool OnImport( AssetImportContext& a_Context ) override;

	};

} // namespace Tridium

#endif // USE_ASSET_IMPORTERS