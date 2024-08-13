#pragma once
#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/Rendering/Material.h>

namespace Tridium::Editor {

    class MaterialEditorPanel : public Panel
    {
    public:
        MaterialEditorPanel( const AssetRef<Material>& a_Material = nullptr );

        virtual void OnImGuiDraw() override;
        virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

        void SetMaterial( const AssetRef<Material>& a_Material ) { m_Material = a_Material; }

    private:
        AssetRef<Material> m_Material;
    };

}

#endif // IS_EDITOR