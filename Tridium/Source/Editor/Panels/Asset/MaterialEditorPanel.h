#pragma once
#if IS_EDITOR

#include <Editor/Panels/Panel.h>
#include <Tridium/Rendering/Material.h>

namespace Tridium::Editor {

    class MaterialEditorPanel : public Panel
    {
    public:
		MaterialEditorPanel();
        MaterialEditorPanel( const MaterialHandle& a_Material );

        virtual void OnImGuiDraw() override;
        virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

        void SetMaterial( MaterialHandle a_Material );
		MaterialHandle GetMaterial() const { return m_Material; }

    private:
        MaterialHandle m_Material;
        bool m_Modified = false;
    };

}

#endif // IS_EDITOR