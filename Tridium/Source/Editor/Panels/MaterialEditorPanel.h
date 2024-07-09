#pragma once
#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/Rendering/Material.h>

namespace Tridium::Editor {

    class MaterialEditorPanel : public Panel
    {
    public:
        MaterialEditorPanel( const MaterialHandle& material = {} );

        virtual void OnImGuiDraw() override;
        virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

        void SetMaterial( const MaterialHandle& material ) { m_Material = material; }

    private:
        MaterialHandle m_Material;
    };

}

#endif // IS_EDITOR