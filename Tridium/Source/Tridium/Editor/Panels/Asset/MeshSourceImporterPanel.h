#pragma once
#if IS_EDITOR
#include <Tridium/Core/Layer.h>
#include <Tridium/Graphics/Rendering/Mesh.h>
#include <Tridium/Asset/AssetMetaData.h>

namespace Tridium {

    class MeshSourceImporterPanel : public Layer
    {
    public:
        MeshSourceImporterPanel( const FilePath& a_Path );
        virtual void OnImGuiDraw() override;

    protected:
        void ImportMeshes();

    protected:
		FilePath m_Path;
		SharedPtr<MeshSource> m_MeshSource;

		std::vector<bool> m_SelectedSubmeshes;

		bool m_ShouldClose = false;
    };

}
#endif // IS_EDITOR
