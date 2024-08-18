#pragma once
#ifdef IS_EDITOR
#include "Panel.h"
#include <Tridium/Asset/Meta/ModelMetaData.h>

namespace Tridium { 
    class GameObject;
    class Mesh;
}

namespace Tridium::Editor {

    class MeshImporterPanel : public Panel
    {
    public:
        MeshImporterPanel( const std::string& filePath );
        MeshImporterPanel( const std::string& filePath, const SharedPtr<Mesh>& mesh );
        virtual void OnImGuiDraw() override;

    private:
        std::string m_FilePath;
        SharedPtr<Mesh> m_Mesh;

        ModelMetaData m_ModelMetaData;
    };

}
#endif // IS_EDITOR
