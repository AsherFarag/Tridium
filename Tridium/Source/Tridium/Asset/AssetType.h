#pragma once

namespace Tridium {

    using AssetHandle = GUID;

    enum class EAssetType : unsigned char
    {
        None = 0,
        // Rendering
        Mesh, Shader, Texture, Material,
        Folder,
        // Scripting
        Lua,
        // Tridium
        Project, Scene
    };

}