#include "tripch.h"
#if IS_EDITOR
#include "MeshSourceImporterPanel.h"
#include <Tridium/Graphics/Rendering/Mesh.h>
#include <Tridium/Asset/Loaders/AssimpImporter.h>
#include <Tridium/Core/Application.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/ImGui/ImGui.h>

namespace Tridium::Editor {



	MeshSourceImporterPanel::MeshSourceImporterPanel( const FilePath& a_Path )
		: Layer( "Mesh Source Importer" )
	{
		m_Path = AssetManager::Get<EditorAssetManager>()->GetAbsolutePath( a_Path );
		AssimpImporter importer( m_Path );
		m_MeshSource = importer.ImportMeshSource();
		//if ( m_MeshSource )
			//m_SelectedSubmeshes.resize( m_MeshSource->m_MeshNodes.size() );

		m_ShouldClose = m_MeshSource == nullptr;
	}

	void MeshSourceImporterPanel::OnImGuiDraw()
	{
		if ( m_ShouldClose )
		{
			Application::Get()->PopOverlay( this, true );
			return;
		}

		ImGui::OpenPopup( "Import MeshSource" );

		ImGui::SetNextWindowPos( ImVec2( 0.5f * ImGui::GetIO().DisplaySize.x, 0.5f * ImGui::GetIO().DisplaySize.y ), ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );

		if ( !ImGui::BeginPopupModal( "Import MeshSource", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize ) )
			return;

		// Display the submesh nodes
		//for ( uint32_t i = 0; i < m_MeshSource->m_MeshNodes.size(); ++i )
		//{
		//	auto& node = m_MeshSource->m_MeshNodes[i];
		//	if ( !node.IsRoot() )
		//		continue;

		//	bool isSelected = m_SelectedSubmeshes[i];
		//	ImGui::Checkbox( node.Name.c_str(), &isSelected );
		//	m_SelectedSubmeshes[i] = isSelected;
		//}

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
				m_ShouldClose = true;
			}

			ImGui::SameLine();

			if ( ImGui::Button( "Cancel", buttonSize ) )
			{
				ImGui::CloseCurrentPopup();
				m_ShouldClose = true;
			}

			ImGui::PopStyleVar();
		}

		ImGui::EndPopup();
	}

	void MeshSourceImporterPanel::ImportMeshes()
	{
		auto assetManager = AssetManager::Get<EditorAssetManager>();
		AssetMetaData meshSourceMetaData =
		{
			AssetHandle::Create(),
			EAssetType::MeshSource,
			m_Path,
			m_Path.GetFilename(),
			true
		};

		// Add the MeshSource to the asset manager
		assetManager->CreateAsset( meshSourceMetaData, m_MeshSource );

		//// Create a single static mesh for the mesh source
		SharedPtr<StaticMesh> mesh = MakeShared<StaticMesh>( m_MeshSource->GetHandle() );

		AssetMetaData metaData;
		metaData.Handle = AssetHandle::Create();
		metaData.AssetType = EAssetType::StaticMesh;
		metaData.Path = m_Path.GetParentPath();
		metaData.Path.Append( meshSourceMetaData.Name + ".tmesh" );
		metaData.Name = meshSourceMetaData.Name;
		metaData.IsAssetLoaded = true;

		assetManager->CreateAsset( metaData, mesh );
		assetManager->RegisterDependency( mesh->GetHandle(), m_MeshSource->GetHandle() );



		// Create a new StaticMesh for each selected submesh
		//for ( uint32_t i = 0; i < m_SelectedSubmeshes.size(); ++i )
		//{
		//	if ( !m_SelectedSubmeshes[i] )
		//		continue;

		//	std::vector<uint32_t> subMeshes = { i };
		//	subMeshes.reserve( m_MeshSource->m_MeshNodes.size() );

		//	// Find all submeshes that are children of the selected submesh
		//	for ( uint32_t j = 0; j < m_MeshSource->m_MeshNodes.size(); ++j )
		//	{
		//		uint32_t parent = m_MeshSource->m_MeshNodes[j].Parent;
		//		while ( parent != MAXUINT32 )
		//		{
		//			if ( parent == i )
		//			{
		//				subMeshes.push_back( j );
		//				break;
		//			}

		//			parent = m_MeshSource->m_MeshNodes[parent].Parent;
		//		}
		//	}

		//	SharedPtr<StaticMesh> mesh = MakeShared<StaticMesh>( m_MeshSource->GetHandle(), subMeshes);

		//	AssetMetaData metaData;
		//	metaData.Handle = AssetHandle::Create();
		//	metaData.AssetType = EAssetType::StaticMesh;
		//	metaData.Path = m_Path.GetParentPath();
		//	metaData.Path.Append( m_MeshSource->m_MeshNodes[i].Name + ".tmesh" );
		//	metaData.Name = m_MeshSource->m_MeshNodes[i].Name;
		//	metaData.IsAssetLoaded = true;

		//	assetManager->CreateAsset( metaData, mesh );
		//	assetManager->RegisterDependency( mesh->GetHandle(), m_MeshSource->GetHandle()  );
		//}
	}

}

#endif // IS_EDITOR