#pragma once

#ifdef IS_EDITOR
#include <Editor/Panels/Panel.h>
#include <Tridium/Asset/AssetType.h>

namespace Tridium::Editor {

    class AssetImporterPanel : public Panel
    {
    public:
        AssetImporterPanel( const char* a_PanelName, const AssetHandle& a_AssetHandle, const IO::FilePath& a_Path )
            : Panel( a_PanelName ), m_AssetHandle( a_AssetHandle ), m_Path( a_Path ) {}

        virtual void OnImGuiDraw() = 0;

    protected:
        AssetHandle m_AssetHandle;
        IO::FilePath m_Path;
    };

}
#endif // IS_EDITOR