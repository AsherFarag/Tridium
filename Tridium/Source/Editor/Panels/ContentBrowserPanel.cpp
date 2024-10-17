#include "tripch.h"
#ifdef IS_EDITOR

#include "ContentBrowserPanel.h"
#include "Editor/Editor.h"
#include <fstream>
#include <Tridium/Core/Application.h>
#include "ScriptEditorPanel.h"

#include <Tridium/Rendering/Material.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>

namespace Tridium::Editor {

	ContentBrowserPanel::ContentBrowserPanel()
		: Panel( "Content Browser" ), m_CurrentDirectory( Application::Get().GetAssetDirectory() )
	{
		IO::FilePath iconFolder( "../Tridium/Content/Engine/Editor/Icons" );

		m_DefaultIcon = TextureLoader::LoadTexture( iconFolder / "file.png" );
		m_FolderIcon = TextureLoader::LoadTexture( iconFolder / "folder.png" );
		m_LuaIcon = TextureLoader::LoadTexture( iconFolder / "file-code.png" );
		m_ImageMediaIcon = TextureLoader::LoadTexture( iconFolder / "file-media.png" );
		m_TridiumProjectIcon = TextureLoader::LoadTexture( iconFolder / "EngineIcon.png" );
	}

	static void DrawDirectoryPath( IO::FilePath& a_Path )
	{
		std::list<IO::FilePath> parentFolderPaths;
		parentFolderPaths.push_back( a_Path );

		while ( parentFolderPaths.front().HasParentPath() )
		{
			parentFolderPaths.push_front( parentFolderPaths.front().GetParentPath() );
		}

		ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 0.f );
		ImGui::PushStyleColor( ImGuiCol_Button, ImGui::GetStyleColorVec4( ImGuiCol_::ImGuiCol_WindowBg ) );
		char uniqueID = 0;
		for ( auto& path : parentFolderPaths )
		{
			if ( ImGui::Button( ( path.GetFilename().ToString() + "##" + std::to_string( uniqueID ) ).c_str() ) )
			{
				a_Path = path;
			}
			uniqueID++;

			if (uniqueID < parentFolderPaths.size() )
			{
				ImGui::SameLine();
				ImGui::Text( ">" );
				ImGui::SameLine();
			}
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::Separator();
	}

	void ContentBrowserPanel::OnImGuiDraw()
	{
		if ( !ImGui::Begin( m_Name.c_str(), &m_Open ) )
		{
			ImGui::End();

			if ( !m_Open )
				Close();

			return;
		}

		// If the directory was deleted while we are in it,
		// Goto to the main content directory.
		if ( !m_CurrentDirectory.Exists() )
			m_CurrentDirectory = "Content";

		DrawDirectoryPath( m_CurrentDirectory );

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)( panelWidth / cellSize );
		if ( columnCount < 1 )
			columnCount = 1;

		if ( ImGui::BeginTable( "Folder Contents Items", columnCount ) )
		{
			for ( auto& directoryEntry : IO::DirectoryIterator( m_CurrentDirectory ) )
			{
				//const auto& path = directoryEntry.path();
				//EAssetType type = IO::GetAssetTypeFromExtension( path.extension().string() );
				//if ( type == EAssetType::None )
				//	continue;

				//ImGui::TableNextColumn();
				//ContentItemOnImGuiDraw( type, path, { thumbnailSize, thumbnailSize } );


				//if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
				//{
				//	ContentOnOpened( type, path );
				//}
			}

			ImGui::EndTable();
		}

		// If the folder is right clicked, open a prompt to make a new file
		if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
			ImGui::OpenPopup("Create Asset", ImGuiPopupFlags_::ImGuiPopupFlags_MouseButtonRight);
		if ( ImGui::BeginPopup( "Create Asset" ) )
		{
			//if ( ImGui::MenuItem( "New Material" ) )
			//{
			//	auto mat = MaterialLibrary::GetMaterial( MaterialLibrary::Create( ( m_CurrentDirectory / "NewMaterial.tasset" ).string() ) );
			//	MaterialSerializer s( mat );
			//	s.SerializeText( mat->GetPath() );

			//	ImGui::CloseCurrentPopup();
			//}

			//char fileName[ 1024 ] = { "ComponentTemplate.lua" };
			//ImGui::InputText( "File Name", fileName, 1024 );

			//ImGui::SameLine();
			//if ( ImGui::Button( "New" ) )
			//{
			//	std::string filePath = ( m_CurrentDirectory / fileName ).string();
			//	std::ifstream existingFile( filePath );
			//	// If there is no file at the filePath,
			//	// make a new file.
			//	if ( !existingFile )
			//	{
			//		std::ofstream newFile( filePath, std::ios::out | std::ios::app );
			//		// Create a new file and write in a component template
			//		newFile << "function OnConstruct()\nend\n\nfunction OnUpdate( deltaTime )\nend\n\nfunction OnDestroy()\nend\n";
			//		newFile.close();
			//	}
			//	existingFile.close();

			//	ImGui::CloseCurrentPopup();
			//}

			//ImGui::SameLine();
			//if ( ImGui::Button( "Cancel" ) )
			//	ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		ImGui::End();

		if ( !m_Open )
			Close();
	}

	bool ContentBrowserPanel::ContentItemOnImGuiDraw( const EFileType a_Type, const IO::FilePath& a_FilePath, const ImVec2& a_Size )
	{
		SharedPtr<Texture> icon;

		// Set Icon
		switch ( a_Type )
		{
			using enum EFileType;
			case Folder:
			{
				icon = m_FolderIcon;
				break;
			}
			case Lua:
			{
				icon = m_LuaIcon;
				break;
			}
			case Image:
			{
				icon = m_ImageMediaIcon;
				break;
			}
			case Tridium_Project:
			{
				icon = m_TridiumProjectIcon;
				break;
			}
		default:
			icon = m_DefaultIcon;
			break;
		}

		// Create an Image Button with the file name at the bottom
		ImGui::BeginGroup();
			float paddingX = 25.f;
			float paddingY = 25.f;
			paddingX = a_Size.x < paddingX ? 0 : paddingX;
			paddingY = a_Size.y < paddingY ? 0 : paddingY;

			ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_FramePadding, { paddingX, paddingY } );
			ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 1.f, 1.f, 1.f, 1.f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 1.f, 1.f, 1.f, 0.5f ) );

			bool result = ImGui::ImageButton(
				a_FilePath.ToString().c_str(),
				(ImTextureID)icon->GetRendererID(),
				{ a_Size.x - paddingX * 2, a_Size.y - paddingY * 2 },
				{ 0,1 }, { 1,0 } );

			ImGui::PopStyleVar();
			ImGui::PopStyleColor(3);

			if ( ImGui::BeginDragDropSource() )
			{
				ImGui::SetDragDropPayload( TE_PAYLOAD_CONTENT_BROWSER_ITEM, a_FilePath.ToString().c_str(), a_FilePath.ToString().size() + 1 );
				ImGui::Image( (ImTextureID)icon->GetRendererID(),
					{ a_Size.x - paddingX * 2, a_Size.y - paddingY * 2 },
					{ 0,1 }, { 1,0 } );

				ImGui::PushTextWrapPos( a_Size.x - paddingX * 2 );
				ImGui::Text( a_FilePath.GetFilename().ToString().c_str());
				ImGui::PopTextWrapPos();

				ImGui::EndDragDropSource();
			}

			ImGui::PushTextWrapPos( ImGui::GetCursorPosX() + a_Size.x );
			ImGui::Text( a_FilePath.GetFilename().ToString().c_str() );
			ImGui::PopTextWrapPos();

		ImGui::EndGroup();

		return result;
	}

	void ContentBrowserPanel::ContentOnOpened( const EFileType type, const IO::FilePath& a_FilePath )
	{
	}
}

#endif // IS_EDITOR