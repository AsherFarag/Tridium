#pragma once
#ifdef IS_EDITOR

#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium::Editor::Util
{
	MeshHandle GetMeshHandle( const std::string& filePath );
	TextureHandle GetTextureHandle( const std::string& filePath );
	MaterialHandle GetMaterialHandle( const std::string& filePath );
};

#endif // IS_EDITOR