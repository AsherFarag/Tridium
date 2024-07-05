#pragma once
#ifdef IS_EDITOR

#include <Tridium/Rendering/Mesh.h>

namespace Tridium::Editor::Util
{
	MeshHandle GetMeshHandle( const std::string& filePath );
};

#endif // IS_EDITOR