#pragma once
#include <Tridium/Asset/AssetImporter.h>

#if USE_ASSET_IMPORTERS

#include <Tridium/Graphics/RHI/RHIDefinitions.h>

namespace Tridium {

	//=============================================================================================
	// TextureImporter: 
	// Imports texture files (e.g. PNG, JPG, TGA) into the engine's internal texture format.
	//=============================================================================================
	DEFINE_ASSET_IMPORTER( TextureImporter )
	{
	public:

		//=============================================================================================
		VersionID Version() const override { return 0; }

		//=============================================================================================
		Array<StringView> SupportedExtensions() const override { return { "png", "jpg", "jpeg", "tga", "bmp" }; }

		//=============================================================================================
		bool OnImport( AssetImportContext& a_Context ) override;

		//=============================================================================================
		// Loads image data from a file into raw pixel data.
		TODO( "Probably temp and should be moved to a utility class" );
		Expected<void, String> LoadFromFile( const char* a_FilePath,
						   Array<byte_t>& o_Data, uint32_t& o_Width, uint32_t& o_Height, ERHIFormat& o_Format, bool& o_IsFloat,
						   bool a_FlipOnLoad = false, int a_DesiredChannels = 0 );

	};

}

#endif