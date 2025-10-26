#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/UUID.h>
#include <Tridium/Core/Enum.h>
#include <Tridium/Core/Version.h>
#include <Tridium/Graphics/Color.h>

namespace Tridium {

	#define ASSET_EXTENSION_NAME "tasset"
	constexpr StringView AssetExtensionName = ASSET_EXTENSION_NAME;

	//=================================================================================================
	// Asset Type ID: Hash of the asset type (e.g., Tridium::Texture, Tridium::Material, etc.)
	// used to identify the type of asset and find the appropriate asset loader.
	//=================================================================================================
	using AssetTypeID = hash_t;
	inline constexpr AssetTypeID InvalidAssetTypeID = 0;

	//=================================================================================================
	// Asset Importer ID: Hash of the asset importer class (e.g., Tridium::FBXImporter)
	//=================================================================================================
	using AssetImporterID = hash_t;
	inline constexpr AssetImporterID InvalidAssetImporterID = 0;

	//=================================================================================================
	// Asset flags: Bit flags for indicating the state of an asset.
	//=================================================================================================
	enum class EAssetFlags : uint32_t
	{
		None = 0,
		LoadedFromDisk = 1 << 0, // The asset was loaded from disk and is not memory-only.
		Persistent = 1 << 2,     // The asset should never be unloaded from memory.
	};
	DEFINE_ENUM_BITMASK_OPERATORS( EAssetFlags );

	//=================================================================================================
	// Asset Load Policy: Describes how and when an asset should be loaded.
	//=================================================================================================
	enum class EAssetLoadPolicy
	{
		Default = 0,      // When the asset is requested, it will load either immediately or asynchronously based on the request.
		ImmediateOnly,    // When the asset is requested, it will always load immediately on the main thread.
		AsyncOnly,        // When the asset is requested, it will always load asynchronously.
		ImmediateOnStart, // It will load immediately on the main thread during the startup phase.
		AsyncOnStart,     // It will load asynchronously during the startup phase.
	};

	//=================================================================================================
	// Asset Type Info: Metadata about an asset type, mostly used for editor display purposes.
	//=================================================================================================
	struct AssetTypeInfo
	{
		AssetTypeID ID = InvalidAssetTypeID;
		String Name{};
		Color4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		StringView Icon{};

		auto& SetName( String a_Name ) { Name = std::move( a_Name ); return *this; }
		auto& SetColor( const Color4& a_Color ) { Color = a_Color; return *this; }
		auto& SetIcon( StringView a_Icon ) { Icon = a_Icon; return *this; }
	};

	//=================================================================================================
	// Asset Header Flags: Special flags for the asset header.
	//=================================================================================================
	enum class EAssetHeaderFlags : uint32_t
	{
		None = 0,
		Compressed = 1 << 0, // The asset data is compressed.
		Encrypted = 1 << 1,  // The asset data is encrypted.
	};

	//=================================================================================================
	// Asset Header: Metadata about an asset stored at the beginning of the asset file.
	//=================================================================================================
	struct AssetHeader
	{
		VersionID Version = UnknownVersionID;
		EAssetHeaderFlags Flags = EAssetHeaderFlags::None;
		AssetTypeID AssetType = InvalidAssetTypeID;
		size_t DataByteSize = 0;
	};

} // namespace Tridium