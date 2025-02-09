#pragma once
#if IS_EDITOR
#include <Tridium/Graphics/Rendering/Texture.h>
#include <Tridium/Asset/AssetType.h>
#include <Tridium/IO/FilePath.h>

namespace Tridium::Editor {

	class AssetImporter final
	{
	public:
		// Will attempt to return a valid asset handle. 
		// However, some assets may open a panel to import the asset.
		// In this case, the function will return an invalid asset handle.
		static AssetHandle ImportAsset( const FilePath& a_FilePath );
	};

}

#endif