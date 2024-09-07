#pragma once
#include "AssetLoader.h"
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	class TextureLoader
	{
		friend class TextureLoaderInterface;
	public:
		static Texture* Load( const IO::FilePath& a_Path );

	private:
		static TextureMetaData* ConstructMetaData();
		static Texture* RuntimeLoad( const IO::FilePath& a_Path );
		static Texture* DebugLoad( const IO::FilePath& a_Path, const TextureMetaData* a_MetaData );
		static bool Save( const IO::FilePath& a_Path, const Texture* a_Asset );
	};

}