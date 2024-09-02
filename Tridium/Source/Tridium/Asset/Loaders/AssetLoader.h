#pragma once
#include <Tridium/Asset/AssetType.h>
#include <Tridium/IO/FilePath.h>

namespace Tridium {

	class Asset;
	class AssetHandle;

	class IAssetLoader
	{
	public:
		virtual Asset* Load( const IO::FilePath& a_Path ) = 0;

#ifdef IS_EDITOR
		virtual AssetHandle Import( const IO::FilePath& a_Path ) = 0;
#endif // IS_EDITOR

	};

}