#pragma once
#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/Rendering/Material.h>

namespace Tridium::Editor {

    class MaterialEditorPanel : public Panel
    {
    public:
        MaterialEditorPanel( const SharedPtr<Material>& material = nullptr );

        virtual void OnImGuiDraw() override;
        virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

        void SetMaterial( const SharedPtr<Material>& material ) { m_Material = material; }

    private:
        SharedPtr<Material> m_Material;
    };

}

#endif // IS_EDITOR