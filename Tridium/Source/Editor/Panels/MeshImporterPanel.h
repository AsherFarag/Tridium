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
        MeshImporterPanel( const IO::FilePath& a_Path );
        virtual void OnImGuiDraw() override;

    private:
        std::string m_Path;
        ModelMetaData m_ModelMetaData;
    };

}
#endif // IS_EDITOR
