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
	}

	void CheckboxFlag( const char* label, unsigned int& flags, const unsigned int flag )
	{
		bool hasFlag = ( flags & flag ) != 0;
		ImGui::Checkbox( label, &hasFlag );
		if ( hasFlag ) { flags |= flag; }
		else { flags &= ~flag; }
	}

	void CheckboxFlag( const char* label, unsigned int& flags, const unsigned int flag, const unsigned int oppositeFlag )
	{
		bool hasFlag = ( flags & flag ) != 0;
		ImGui::Checkbox( label, &hasFlag );
		if ( hasFlag ) { flags |= flag; flags &= ~oppositeFlag; }
		else { flags |= oppositeFlag; flags &= ~flag; }
	}

	void MeshImporterPanel::OnImGuiDraw()
	{
		if ( ImGui::Begin( m_Name.c_str() ) )
		{
			ImGui::Text( "File Path: %s", m_FilePath.c_str() );

			ImGui::Separator();

			if ( ImGui::TreeNodeEx( "Import Settings", ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				if ( ImGui::TreeNodeEx( "Post-Process Flags", ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					CheckboxFlag( "Smooth Normals", m_ImportSettings.PostProcessFlags, aiProcess_GenSmoothNormals, aiProcess_GenNormals );
					CheckboxFlag( "Fix Infacing Normals", m_ImportSettings.PostProcessFlags, aiProcess_FixInfacingNormals );

					ImGui::TreePop();
				}

				ImGui::Checkbox( "Keep Local Mesh Data", &m_ImportSettings.DiscardLocalData );
				ImGui::DragFloat( "Scale", &m_ImportSettings.Scale );

				ImGui::TreePop();
			}

			ImGui::Separator();

			if ( ImGui::Button( "LOAD", { ImGui::GetContentRegionAvail().x * 0.5f, 40 } ) )
			{
				if ( Ref<Mesh> mesh = MeshLoader::Import( m_FilePath, m_ImportSettings ) )
				{
					MeshHandle handle = m_MeshHandle.Valid() ? m_MeshHandle : GUID::Create();
					MeshLibrary::AddMesh( m_FilePath, mesh, handle );
				}
				else
				{
					TE_CORE_ERROR( "Failed to import mesh from '{0}'", m_FilePath );
				}

				m_Open = false;
			}

			ImGui::SameLine();

			ImGui::PushStyleColor( ImGuiCol_Button, { 1.0f, 0.4f, 0.2f, 1.0f } );
			if ( ImGui::Button( "Cancel", { ImGui::GetContentRegionAvail().x, 40 } ) )
			{
				m_Open = false;
			}
			ImGui::PopStyleColor();
		}

		ImGuiEnd();
	}
}

#endif // IS_EDITOR