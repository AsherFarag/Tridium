#pragma once
#ifdef IS_EDITOR
#include <Tridium/Core/Layer.h>
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Asset/AssetMetaData.h>

namespace Tridium::Editor {

    class MeshSourceImporterPanel : public Layer
    {
    public:
        MeshSourceImporterPanel( const IO::FilePath& a_Path );
        virtual void OnImGuiDraw() override;

    protected:
        void ImportMeshes();

    protected:
		IO::FilePath m_Path;
		SharedPtr<MeshSource> m_MeshSource;

		std::vector<bool> m_SelectedSubmeshes;

		bool m_ShouldClose = false;
    };

}
#endif // IS_EDITOR
