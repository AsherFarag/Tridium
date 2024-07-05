#include "tripch.h"
#ifdef IS_EDITOR
#include "MeshImporterPanel.h"
#include <assimp/postprocess.h>

namespace Tridium::Editor {

	MeshImporterPanel::MeshImporterPanel( const std::string& filePath )
		: Panel("Mesh Importer"), m_FilePath(filePath)
	{

	}

	MeshImporterPanel::MeshImporterPanel( const std::string& filePath, MeshHandle meshHandle )
		: MeshImporterPanel(filePath)
	{
		m_MeshHandle = meshHandle;
		m_HasMeshHandle = true;
	}

	void MeshImporterPanel::OnImGuiDraw()
	{
		if ( ImGui::Begin( m_Name.c_str() ) )
		{
			ImGui::Text( "File Path: %s", m_FilePath.c_str() );

			ImGui::Separator();

			if ( ImGui::TreeNode( "Import Settings" ) )
			{
				if ( ImGui::TreeNode( "Post-Process Flags" ) )
				{
					// Generate Smooth Normals
					bool smoothNormals = ( m_ImportSettings.PostProcessFlags & aiProcess_GenSmoothNormals ) != 0;
					ImGui::Checkbox( "Smooth Normals", &smoothNormals );
					if ( smoothNormals ) { m_ImportSettings.PostProcessFlags |= aiProcess_GenSmoothNormals; m_ImportSettings.PostProcessFlags &= ~aiProcess_GenNormals; }
					else { m_ImportSettings.PostProcessFlags |= aiProcess_GenNormals; m_ImportSettings.PostProcessFlags &= ~aiProcess_GenSmoothNormals; }

					// Fix In-Facing Normals
					bool inFacingNormals = ( m_ImportSettings.PostProcessFlags & aiProcess_FixInfacingNormals ) != 0;
					ImGui::Checkbox( "Fix Infacing Normals", &inFacingNormals );
					if ( inFacingNormals ) { m_ImportSettings.PostProcessFlags |= aiProcess_FixInfacingNormals; }
					else { m_ImportSettings.PostProcessFlags &= ~aiProcess_FixInfacingNormals; }

					ImGui::TreePop();
				}

				ImGui::Checkbox( "Keep Local Mesh Data", &m_ImportSettings.DiscardLocalData );

				ImGui::DragFloat( "Scale", &m_ImportSettings.Scale );

				ImGui::TreePop();
			}

			ImGui::Separator();

			if ( ImGui::Button( "LOAD", { ImGui::GetContentRegionAvail().x, 40 } ) )
			{
				MeshHandle handle;
				MeshLoader::Import( m_FilePath, handle, m_ImportSettings );
				m_Open = false;
			}
		}

		ImGuiEnd();
	}
}

#endif // IS_EDITOR