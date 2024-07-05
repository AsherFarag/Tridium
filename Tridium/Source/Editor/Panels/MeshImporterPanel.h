#pragma once
#ifdef IS_EDITOR
#include "Panel.h"
#include <Tridium/Rendering/Mesh.h>

namespace Tridium::Editor {

    class MeshImporterPanel : public Panel
    {
    public:
        MeshImporterPanel( const std::string& filePath );
        MeshImporterPanel( const std::string& filePath, MeshHandle meshHandle );
        virtual void OnImGuiDraw() override;

    private:
        std::string m_FilePath;
        MeshHandle m_MeshHandle;
        bool m_HasMeshHandle = false;
        MeshImportSettings m_ImportSettings;
    };

}
#endif // IS_EDITOR
