#pragma once
#ifdef IS_EDITOR

#include <Editor/Panels/Panel.h>
#include <Tridium/Rendering/Material.h>

namespace Tridium::Editor {

    class MaterialEditorPanel : public Panel
    {
    public:
		MaterialEditorPanel();
        MaterialEditorPanel( const AssetHandle& a_Material );

        virtual void OnImGuiDraw() override;
        virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

        void SetMaterial( const AssetHandle& a_Material ) { m_Material = a_Material; }

    private:
        AssetHandle m_Material;
        bool m_Modified = false;
    };

}

#endif // IS_EDITOR