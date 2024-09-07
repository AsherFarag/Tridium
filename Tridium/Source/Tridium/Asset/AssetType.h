#pragma once
#include <Tridium/Core/Types.h>

namespace Tridium {

    using AssetHandle = GUID;

    enum class EAssetType : Byte
    {
        None = 0,
        // Rendering
        Mesh, Shader, Texture, Material,
        // Misc
        Folder,
        // Scripting
        Lua,
        // Tridium
        Project, Scene
    };

}