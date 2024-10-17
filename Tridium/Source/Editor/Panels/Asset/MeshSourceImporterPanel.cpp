#include "tripch.h"
#ifdef IS_EDITOR
#include "MeshSourceImporterPanel.h"
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Asset/Loaders/AssimpImporter.h>
#include <Tridium/Core/Application.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/ImGui/ImGui.h>

namespace Tridium::Editor {



	MeshSourceImporterPanel::MeshSourceImporterPanel( const IO::FilePath& a_Path )
		: Layer( "Mesh Source Importer" )
	{
		m_Path = a_Path;
		AssimpImporter importer( m_Path );
		m_MeshSource = importer.ImportMeshSource();
		m_SelectedSubmeshes.resize( m_MeshSource->m_MeshNodes.size() );
	}

	void MeshSourceImporterPanel::OnImGuiDraw()
	{
		ImGui::OpenPopup( "Import MeshSource" );

		ImGui::SetNextWindowPos( ImVec2( 0.5f * ImGui::GetIO().DisplaySize.x, 0.5f * ImGui::GetIO().DisplaySize.y ), ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );

		if ( !ImGui::BeginPopupModal( "Import MeshSource", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize ) )
			return;

		bool shouldClose = false;
		shouldClose |= m_MeshSource == nullptr; // Close the window if the mesh source failed to load

		// Display the submesh nodes
		for ( uint32_t i = 0; i < m_MeshSource->m_MeshNodes.size(); ++i )
		{
			auto& node = m_MeshSource->m_MeshNodes[i];
			//if ( !node.IsRoot() )
			//	continue;

			bool isSelected = m_SelectedSubmeshes[i];
			ImGui::Checkbox( node.Name.c_str(), &isSelected );
			m_SelectedSubmeshes[i] = isSelected;
		}

		ImGui::Separator();

		// Import and Cancel buttons
		{
			ImVec2 buttonSize = ImGui::CalcTextSize( "Cancel" );
			const ImVec2 buttonPadding = ImGui::GetStyle().FramePadding;
			buttonSize.x += buttonPadding.x * 2.0f;
			buttonSize.y += buttonPadding.y * 2.0f;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1.0f );

			if ( ImGui::Button( "Import", buttonSize ) )
			{
				ImportMeshes();
				ImGui::CloseCurrentPopup();
				shouldClose = true;
			}

			ImGui::SameLine();

			if ( ImGui::Button( "Cancel", buttonSize ) )
			{
				ImGui::CloseCurrentPopup();
				shouldClose = true;
			}

			ImGui::PopStyleVar();
		}

		ImGui::EndPopup();

		if ( shouldClose )
		{
			Application::Get().PopOverlay( this, true );
		}
	}

	void MeshSourceImporterPanel::ImportMeshes()
	{
		auto assetManager = AssetManager::Get<EditorAssetManager>();
		AssetMetaData meshSourceMetaData;
		meshSourceMetaData.Handle = AssetHandle::Create();
		meshSourceMetaData.AssetType = EAssetType::MeshSource;
		meshSourceMetaData.Path = m_Path;
		meshSourceMetaData.Name = m_Path.GetFilename();
		meshSourceMetaData.IsAssetLoaded = true;

		// Add the MeshSource to the asset manager
		assetManager->CreateAsset( meshSourceMetaData, m_MeshSource );

		// Create a new StaticMesh for each selected submesh
		for ( uint32_t i = 0; i < m_SelectedSubmeshes.size(); ++i )
		{
			if ( !m_SelectedSubmeshes[i] )
				continue;

			std::vector<uint32_t> subMeshes = { i };
			subMeshes.reserve( m_MeshSource->m_MeshNodes.size() );

			// Find all submeshes that are children of the selected submesh
			for ( uint32_t j = 0; j < m_MeshSource->m_MeshNodes.size(); ++j )
			{
				uint32_t parent = m_MeshSource->m_MeshNodes[j].Parent;
				while ( parent != MAXUINT32 )
				{
					if ( parent == i )
					{
						subMeshes.push_back( j );
						break;
					}

					parent = m_MeshSource->m_MeshNodes[parent].Parent;
				}
			}

			SharedPtr<StaticMesh> mesh = MakeShared<StaticMesh>( m_MeshSource->m_Handle, subMeshes );

			AssetMetaData metaData;
			metaData.Handle = AssetHandle::Create();
			metaData.AssetType = EAssetType::StaticMesh;
			metaData.Path = m_Path;
			metaData.Name = m_MeshSource->m_MeshNodes[i].Name;
			metaData.IsAssetLoaded = true;

			assetManager->CreateAsset( metaData, mesh );
			assetManager->RegisterDependency( m_MeshSource->m_Handle, mesh->GetHandle() );
		}
	}

}

#endif // IS_EDITOR