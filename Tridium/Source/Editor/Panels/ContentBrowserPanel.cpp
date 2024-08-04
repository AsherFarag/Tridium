#include "tripch.h"
#ifdef IS_EDITOR

#include "ContentBrowserPanel.h"
#include "Editor/Editor.h"
#include <fstream>
#include <Tridium/Core/Application.h>
#include "ScriptEditorPanel.h"

#include <Tridium/IO/MaterialSerializer.h>
#include <Tridium/Rendering/Material.h>

namespace Tridium::Editor {

	ContentBrowserPanel::ContentBrowserPanel()
		: Panel( "Content Browser" ), m_CurrentDirectory( Application::Get().GetAssetDirectory() )
	{
		TODO( "Make proper Icon Assets" );
		// TEMP
		fs::path iconFolder( "Content/Engine/Editor/Icons" );
		m_DefaultIcon = Texture::Load( ( iconFolder / "file.png" ).string() );
		m_FolderIcon = Texture::Load( ( iconFolder / "folder.png" ).string() );
		m_LuaIcon = Texture::Load( ( iconFolder / "file-code.png" ).string() );
		m_ImageMediaIcon = Texture::Load( ( iconFolder / "file-media.png" ).string() );
		m_TridiumProjectIcon = Texture::Load( ( iconFolder / "EngineIcon.png" ).string() );
		//m_TridiumSceneIcon = Texture2D::Create( ( iconFolder / "tridium-scene.png" ).string() );
	}

	static void DrawDirectoryPath( fs::path& a_Path )
	{
		std::list<fs::path> parentFolderPaths;
		parentFolderPaths.push_back( a_Path );

		while ( parentFolderPaths.front().has_parent_path() )
		{
			parentFolderPaths.push_front( parentFolderPaths.front().parent_path() );
		}

		ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 0.f );
		char uniqueID = 0;
		for ( auto& path : parentFolderPaths )
		{
			if ( ImGui::Button( ( path.filename().string() + "##" + std::to_string( uniqueID ) ).c_str() ) )
			{
				a_Path = path;
			}
			uniqueID++;

			if (uniqueID < parentFolderPaths.size() )
			{
				ImGui::SameLine();
				ImGui::Text( "/" );
				ImGui::SameLine();
			}
		}

		ImGui::PopStyleVar();
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
		if ( !fs::exists( m_CurrentDirectory ) )
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
			for ( auto& directoryEntry : std::filesystem::directory_iterator( m_CurrentDirectory ) )
			{
				ImGui::TableNextColumn();
				const auto& path = directoryEntry.path();

				ContentType type = GetContentType( path );

				ContentItemOnImGuiDraw( type, path, { thumbnailSize, thumbnailSize } );


				if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
				{
					ContentOnOpened( type, path );
				}
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

	ContentType ContentBrowserPanel::GetContentType( const fs::path& a_FilePath )
	{
		if ( fs::is_directory( a_FilePath ) )
			return ContentType::Folder;

		std::string ext = a_FilePath.extension().string();
		if ( ext == ".lua" ) { return ContentType::Lua; }
		if ( ext == ".TEproject" ) { return ContentType::Tridium_Project; }
		if ( ext == ".png") { return ContentType::Texture; }

		return ContentType::None;
	}

	bool ContentBrowserPanel::ContentItemOnImGuiDraw( const ContentType type, const fs::path& a_FilePath, const ImVec2& size )
	{
		SharedPtr<Texture> icon;

		// Set Icon
		switch ( type )
		{
			case ContentType::Folder:
			{
				icon = m_FolderIcon;
				break;
			}
			case ContentType::Lua:
			{
				icon = m_LuaIcon;
				break;
			}
			case ContentType::Texture:
			{
				icon = m_ImageMediaIcon;
				break;
			}
			case ContentType::Tridium_Project:
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
			paddingX = size.x < paddingX ? 0 : paddingX;
			paddingY = size.y < paddingY ? 0 : paddingY;

			ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_FramePadding, { paddingX, paddingY } );
			ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 1.f, 1.f, 1.f, 1.f ) );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 1.f, 1.f, 1.f, 0.5f ) );

			bool result = ImGui::ImageButton(
				a_FilePath.string().c_str(),
				(ImTextureID)icon->GetRendererID(),
				{ size.x - paddingX * 2, size.y - paddingY * 2 },
				{ 0,1 }, { 1,0 } );

			ImGui::PopStyleVar();
			ImGui::PopStyleColor(3);

			if ( ImGui::BeginDragDropSource() )
			{
				ImGui::SetDragDropPayload( TE_PAYLOAD_CONTENT_BROWSER_ITEM, a_FilePath.string().c_str(), a_FilePath.string().size() + 1 );
				ImGui::Image( (ImTextureID)icon->GetRendererID(),
					{ size.x - paddingX * 2, size.y - paddingY * 2 },
					{ 0,1 }, { 1,0 } );

				ImGui::PushTextWrapPos( size.x - paddingX * 2 );
				ImGui::Text( a_FilePath.filename().string().c_str() );
				ImGui::PopTextWrapPos();

				ImGui::EndDragDropSource();
			}

			ImGui::PushTextWrapPos( ImGui::GetCursorPosX() + size.x );
			ImGui::Text( a_FilePath.filename().string().c_str() );
			ImGui::PopTextWrapPos();

		ImGui::EndGroup();

		return result;
	}

	void ContentBrowserPanel::ContentOnOpened( const ContentType type, const fs::path& a_FilePath )
	{
		switch ( type )
		{
			case ContentType::Folder:
			{
				m_CurrentDirectory = a_FilePath;
				break;
			}
			case ContentType::Lua:
			{
				Util::OpenFile( a_FilePath );
				break;
			}
		default:
			break;
		}
	}
}

#endif // IS_EDITOR