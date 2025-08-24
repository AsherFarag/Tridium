#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/GUID.h>
#include <Tridium/Core/Enum.h>

namespace Tridium {

	using AssetID = GUID; // Alias for asset ID, using GUID for unique identification

	//==============================
	// Asset flags
	//  Bit flags for indicating the state of an asset.
	enum class EAssetFlags : uint32_t
	{
		None = 0,
		MemoryOnly = 1 << 1, // The asset is only stored in memory, not on disk. This is set by the AssetDatabase.
	};
	DEFINE_ENUM_BITMASK_OPERATORS( EAssetFlags );

	//==============================
	// Asset types
	//  Enum of all asset types.
	enum class EAssetType : uint8_t
	{
		Unknown = 0,
		Texture,
		Material,
		MeshSource,
		StaticMesh,
		Sound,
		Scene,
		Script,
		Animation,
		Font,
		COUNT
	};

	enum class EAssetLoadPolicy
	{
		Default = 0,      // When the asset is requested, it will load either immediately or asynchronously based on the request.
		ImmediateOnly,    // When the asset is requested, it will always load immediately on the main thread.
		AsyncOnly,        // When the asset is requested, it will always load asynchronously.
		ImmediateOnStart, // When the asset is requested, it will load immediately on the main thread during the startup phase.
		AsyncOnStart,     // When the asset is requested, it will load asynchronously during the startup phase.
	};

} // namespace Tridium