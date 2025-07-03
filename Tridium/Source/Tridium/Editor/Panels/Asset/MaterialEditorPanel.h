#pragma once
#if IS_EDITOR

#include <Tridium/Editor/Panels/Panel.h>
#include <Tridium/Asset/MaterialAsset.h>

namespace Tridium {

    class MaterialEditorPanel : public Panel
    {
    public:
		MaterialEditorPanel();
        MaterialEditorPanel( AssetID a_Material );

        virtual void OnImGuiDraw() override;
        virtual bool OnKeyPressed( const KeyPressedEvent& e ) override;

        void SetMaterial( AssetID a_Material );
        AssetID GetMaterial() const { return m_Material; }

    private:
        AssetID m_Material;
        bool m_Modified = false;
    };

}

#endif // IS_EDITOR