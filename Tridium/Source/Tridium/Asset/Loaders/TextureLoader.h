#pragma once
namespace Tridium {

	class Texture;
	struct TextureMetaData;

	class TextureLoader
	{
	public:
		static Texture* Load( const IO::FilePath& a_Path );
		static Texture* Load( const IO::FilePath& a_Path, const TextureMetaData& a_MetaData );
	};

}

