#include "tripch.h"
#ifdef IS_EDITOR
#include "MeshImporterPanel.h"
#include <assimp/postprocess.h>
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Asset/Loaders/ModelLoader.h>
#include <Editor/EditorAssetManager.h>

namespace Tridium::Editor {

	MeshImporterPanel::MeshImporterPanel( const IO::FilePath& a_Path )
		: Panel("Mesh Importer"), m_Path( a_Path )
	{

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
			ImGui::Text( "File Path: %s", m_Path.c_str() );

			ImGui::Separator();

			// Draw import settings options

			if ( ImGui::TreeNodeEx( "Import Settings", ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				ImGui::DragFloat( "Scale", &m_ModelMetaData.ImportSettings.Scale );

				ImGui::Separator();

				if ( ImGui::TreeNodeEx( "Post-Process Flags", ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					CheckboxFlag( "Smooth Normals", m_ModelMetaData.ImportSettings.PostProcessFlags, aiProcess_GenSmoothNormals, aiProcess_GenNormals );
					CheckboxFlag( "Fix Infacing Normals", m_ModelMetaData.ImportSettings.PostProcessFlags, aiProcess_FixInfacingNormals );

					ImGui::TreePop();
				}

				ImGui::Separator();

				ImGui::Checkbox( "Keep Local Mesh Data", &m_ModelMetaData.ImportSettings.DiscardLocalData );

				ImGui::TreePop();
			}

			ImGui::Separator();

			if ( ImGui::Button( "Import", { ImGui::GetContentRegionAvail().x * 0.5f, 40 } ) )
			{
				if ( Mesh* mesh = ModelLoader::Load( m_Path, m_ModelMetaData ) )
				{
					EditorAssetManager::AddAsset( mesh );
					m_ModelMetaData.Serialize( m_Path );
					TE_CORE_INFO( "Successfully imported mesh from '{0}'", m_Path );
				}
				else
				{
					TE_CORE_ERROR( "Failed to import mesh from '{0}'", m_Path );
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