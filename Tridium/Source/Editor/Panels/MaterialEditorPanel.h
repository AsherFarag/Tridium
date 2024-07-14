#pragma once
#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/Rendering/Material.h>

namespace Tridium::Editor {

    class MaterialEditorPanel : public Panel
    {
    public:
        MaterialEditorPanel( const Ref<Material>& material = nullptr );

        virtual void OnImGuiDraw() override;
        virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

        void SetMaterial( const Ref<Material>& material ) { m_Material = material; }

    private:
        Ref<Material> m_Material;
    };

}

#endif // IS_EDITOR