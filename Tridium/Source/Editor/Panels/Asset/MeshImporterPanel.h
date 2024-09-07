#pragma once
#ifdef IS_EDITOR
#include "AssetImporterPanel.h"
#include <Tridium/Asset/AssetMetaData.h>

namespace Tridium::Editor {

    class MeshImporterPanel : public AssetImporterPanel
    {
    public:
        MeshImporterPanel( const AssetHandle& a_AssetHandle, const IO::FilePath& a_Path );
        virtual void OnImGuiDraw() override;

    private:
        ModelMetaData m_ModelMetaData;
    };

}
#endif // IS_EDITOR
