#pragma once
#include "Asset.h"
#include <Tridium/Core/Core.h>

namespace Tridium::T {

	// Runtime
	// - Get Asset by ID
	// - Get Asset by Path
	// - Asset Dependencies

	// Editor
	// - Asset Importing
	// - Asset Exporting
	// - Asset Management (e.g., moving, renaming, deleting assets)


	class IAssetManager
	{
	public:
		IAssetManager() = default;
		virtual ~IAssetManager() = default;

		virtual Expected<void, String> Init() = 0;
		virtual Expected<void, String> Shutdown() = 0;

		virtual IAsset* GetAsset( AssetID a_ID ) = 0;
		virtual AssetID GetAssetIDFromPath( StringView a_Path ) = 0;

	};


	class AssetManager
	{
	public:
	};

}