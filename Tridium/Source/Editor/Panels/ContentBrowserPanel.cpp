#include "tripch.h"
#ifdef IS_EDITOR

#include "ContentBrowserPanel.h"
#include "Editor/Editor.h"
#include <fstream>
#include <Tridium/Core/Application.h>
#include "ScriptEditorPanel.h"
#include <Tridium/Asset/EditorAssetManager.h>

#include <Tridium/Rendering/Material.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>
#include <Editor/AssetImporter.h>

namespace Tridium::Editor {

	std::unordered_map<EFileType, SharedPtr<Texture>> ContentItemIcons::s_FileTypeIcons = std::unordered_map<EFileType, SharedPtr<Texture>>();
	SharedPtr<Texture> ContentItemIcons::s_UnimportedAssetIcon = nullptr;

	ContentBrowserPanel::ContentBrowserPanel() : Panel( "Content Browser" )
	{
		IO::FilePath iconFolder( Application::GetEngineAssetsDirectory() / "Editor/Icons" );
		SharedPtr<Texture> defaultIcon = TextureLoader::LoadTexture( iconFolder / "file.png" );
		ContentItemIcons::s_FileTypeIcons = {
			{ EFileType::None,       defaultIcon },
			{ EFileType::Folder,     TextureLoader::LoadTexture( iconFolder / "folder.png" ) },
			{ EFileType::Scene,      TextureLoader::LoadTexture( iconFolder / "tridium-scene.png" ) },
			{ EFileType::Material,   defaultIcon },
			{ EFileType::MeshSource, defaultIcon },
			{ EFileType::StaticMesh, defaultIcon },
			{ EFileType::Shader,     defaultIcon },
			{ EFileType::Texture,    TextureLoader::LoadTexture( iconFolder / "file-media.png" ) },
			{ EFileType::CubeMap,    TextureLoader::LoadTexture( iconFolder / "file-media.png" ) },
			{ EFileType::Lua,	     TextureLoader::LoadTexture( iconFolder / "file-code.png" ) },
		};

		ContentItemIcons::s_UnimportedAssetIcon = TextureLoader::LoadTexture( iconFolder / "file-unimported.png" );
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
			OpenFolder(Application::GetActiveProject()->GetAssetDirectory());

		ImGui::Columns( 2, "Content Browser Columns", true );
		{
			// ImGui does not serialize the column width, so we need to set it once on the first frame.
			static bool s_SetColumnWidth = true;
			if ( s_SetColumnWidth )
			{
				ImGui::SetColumnWidth( 0, 250.0f );
				s_SetColumnWidth = false;
			}

			DrawFolderHierarchy();

			ImGui::NextColumn();

			DrawContentBrowserHeader();

			DrawContentItems();
		}

		DrawContentBrowserSettings();

		ImGui::End();

		if ( !m_Open )
			Close();
	}

	void ContentBrowserPanel::OpenFolder( const IO::FilePath& a_Path )
	{
		m_CurrentDirectory = a_Path;
		m_DirectoryInputBuffer = m_CurrentDirectory.ToString();
		ReconstructDirectoryStack();
		ReconstructFolderHierarchy();
	}

	void ContentBrowserPanel::ReconstructDirectoryStack()
	{
		m_DirectoryStack.clear();

		IO::FilePath parentFolderPaths = m_CurrentDirectory;
		const IO::FilePath assetDirectory = Application::GetActiveProject()->GetAssetDirectory();

		int loopGuard = 0;
		while ( parentFolderPaths.HasParentPath() )
		{
			if ( parentFolderPaths.IsRoot() || ++loopGuard > 100 )
				break;

			// If the parent folder is the asset directory, stop.
			if ( parentFolderPaths == assetDirectory )
				break;

			m_DirectoryStack.push_front( parentFolderPaths.GetFilename().ToString() );
			parentFolderPaths = parentFolderPaths.GetParentPath();
		}

		m_DirectoryStack.push_front( Application::GetActiveProject()->GetConfiguration().AssetDirectory.GetFilename() );
	}

	void ContentBrowserPanel::ReconstructFolderHierarchy()
	{
		m_FolderHeirarchy.clear();
		RecurseFolderHeirarchy( Application::GetActiveProject()->GetAssetDirectory() );
	}

	void ContentBrowserPanel::RecurseFolderHeirarchy( const IO::FilePath& a_Directory )
	{
		for ( auto& directoryEntry : IO::DirectoryIterator( a_Directory ) )
		{
			IO::FilePath filePath = directoryEntry.path();
			std::string fileName = filePath.GetFilename().ToString();

			EFileType type = EFileType::Folder;
			if ( !directoryEntry.is_directory() )
				type = static_cast<EFileType>( GetAssetTypeFromFileExtension( directoryEntry.path().extension().string() ) );

			AssetMetaData metaData = EditorAssetManager::Get()->GetAssetMetaData( filePath );

			bool isImported = metaData.IsValid();
			AssetHandle handle = metaData.Handle;

			// Add the file to the folder heirarchy
			m_FolderHeirarchy[a_Directory].emplace_back( *this, type, fileName, handle, isImported );

			if ( type == EFileType::Folder )
				RecurseFolderHeirarchy( filePath );
		}
	}

	void ContentBrowserPanel::DrawFolderHierarchy()
	{
		ImGui::BeginChild( "Folder Hierarchy", { 0, 0 }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar );
		{
			// Add some padding to the top of the window
			ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 2.0f );
			const std::string& rootFolderName = m_DirectoryStack.front();
			if ( ImGui::TreeNodeEx( ( TE_ICON_FOLDER " " + rootFolderName ).c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Framed) )
			{
				RecurseDrawFolderHierarchy( Application::GetActiveProject()->GetAssetDirectory() );
				ImGui::TreePop();
			}
		}
		ImGui::EndChild();
	}

	void ContentBrowserPanel::RecurseDrawFolderHierarchy( const IO::FilePath& a_Directory )
	{
		if ( m_FolderHeirarchy.find( a_Directory ) == m_FolderHeirarchy.end() )
			return;

		for ( auto& item : m_FolderHeirarchy[a_Directory] )
		{
			const char* icon = item.Type == EFileType::Folder ? TE_ICON_FOLDER " " : TE_ICON_FILE " ";
			std::string itemName = icon + item.Name;

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			if ( item.Type != EFileType::Folder )
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}
			else if ( m_CurrentDirectory == ( a_Directory / item.Name ) )
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			const bool open = ImGui::TreeNodeEx( itemName.c_str(), flags );

			// Open the file if it was double clicked
			if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
			{
				if ( OnContentItemOpened( item ) )
				{
					if ( open ) ImGui::TreePop();
					break;
				}
			}

			if ( !open )
				continue;

			// Item is a folder, recurse into it

			RecurseDrawFolderHierarchy( a_Directory / item.Name );
			ImGui::TreePop();
		}
	}

	void ContentBrowserPanel::DrawContentBrowserHeader()
	{
		// Get the cursor position and frame padding for the input text box
		const ImVec2& framePadding = ImGui::GetStyle().FramePadding;
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += framePadding.x;
		cursorPos.y += framePadding.y;

		//  - Draw the directory path input box - 
		// If the directory input text box is active, set the text color to the default text color
		// Otherwise, set the text color to invisible.
		if ( ImGui::IsItemActive( ImGui::GetID( "##Directory Path" ) ) )
			ImGui::PushStyleColor( ImGuiCol_Text, ImGui::GetStyleColorVec4( ImGuiCol_Text ) );
		else
			ImGui::PushStyleColor( ImGuiCol_Text, ImVec4() );

		// Allow the following folder button to overlap the input text box
		ImGui::SetNextItemAllowOverlap();
		if ( ImGui::InputText( "##Directory Path", &m_DirectoryInputBuffer, ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			IO::FilePath newDirectory = m_DirectoryInputBuffer;
			if ( !newDirectory.IsDirectory() )
			{
				// The newDirectory is not a directory, attempt to open the parent directory
				newDirectory = newDirectory.GetParentPath();
			}

			if ( newDirectory.Exists() )
			{
				OpenFolder( newDirectory );
			}
			else
			{
				// The new directory does not exist, reset the input buffer to the current directory
				m_DirectoryInputBuffer = m_CurrentDirectory.ToString();
			}
		}
		ImGui::PopStyleColor();

		const bool isDirectoryInputActive = ImGui::IsItemActive();

		// Draw Refresh Button
		{
			ImGui::SameLine();
			if ( ImGui::Button( TE_ICON_ROTATE ) )
				ReconstructFolderHierarchy();
		}

		// Draw Seperator
		{
			ImGui::SameLine();
			ImGui::Text( " | " );
		}

		// Draw Content Search Filter
		{
			ImGui::SameLine();
			ImGui::Text( TE_ICON_MAGNIFYING_GLASS );
			ImGui::SameLine();
			float width = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize( TE_ICON_GEAR ).x - ImGui::GetStyle().WindowPadding.x * 2 - 5.0f;
			ImGui::SetNextItemWidth( width );
			ImGui::SetNextItemAllowOverlap();
			ImGui::InputText( "##Content Search Filter", &m_ContentSearchFilter );

			//float buttonWidth = ImGui::CalcTextSize( TE_ICON_X ).x + ImGui::GetStyle().FramePadding.x * 2;
			//ImGui::SameLine( );
			//if ( ImGui::Button( TE_ICON_X ) )
			//	m_ContentSearchFilter.clear();
		}


		// Draw the settings button
		{
			ImGui::SameLine( ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize( TE_ICON_GEAR ).x - ImGui::GetStyle().WindowPadding.x );
			if ( ImGui::Button( TE_ICON_GEAR ) )
				ImGui::OpenPopup( "Content Browser Settings" );
		}


		// If the directory input text box is active, don't draw the directory path buttons
		if ( isDirectoryInputActive )
			return;

		// Draw the directory path buttons
		{
			ImGui::SetCursorScreenPos( cursorPos ); // Reset the cursor position

			ImGui::Text( " " TE_ICON_FOLDER_OPEN " " );
			ImGui::SameLine();

			ImGui::ScopedStyleVar frame( ImGuiStyleVar_FrameBorderSize, 0.f );
			ImGui::ScopedStyleCol buttonCol( ImGuiCol_Button, ImVec4() );
			uint32_t i = 0;
			for ( const std::string& folder : m_DirectoryStack )
			{

				ImGui::ScopedID id( i );
				if ( ImGui::SmallButton( folder.c_str() ) )
				{
					for ( uint32_t j = m_DirectoryStack.size() - 1; j > i; --j )
						m_CurrentDirectory = m_CurrentDirectory.GetParentPath();

					OpenFolder( m_CurrentDirectory );
					break;
				}

				if ( i < m_DirectoryStack.size() - 1 )
				{
					ImGui::SameLine();
					ImGui::Text( ">" );
					ImGui::SameLine();
				}

				i++;
			}
		}
	}

	void ContentBrowserPanel::DrawContentItems()
	{
		if ( !ImGui::BeginChild( "Folder Contents", { 0, 0 }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_AlwaysVerticalScrollbar ) )
		{
			ImGui::EndChild();
			return;
		}

		const float padding = 16.0f;
		const float cellSize = m_ContentThumbnailSize + padding;
		const float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)( panelWidth / cellSize );
		if ( columnCount < 1 )
			columnCount = 1;

		if ( ImGui::BeginTable( "Folder Contents Items", columnCount ) )
		{
			ImGuiTextFilter filter( m_ContentSearchFilter.c_str() );

			for ( auto& item : m_FolderHeirarchy[m_CurrentDirectory] )
			{
				if ( !filter.PassFilter( item.Name.c_str() ) )
					continue;

				ImGui::TableNextColumn();
				const bool wasOpened = item.OnImGuiDraw( { m_ContentThumbnailSize, m_ContentThumbnailSize } );

				if ( wasOpened )
				{
					bool shouldBreak = OnContentItemOpened( item );
					if ( shouldBreak )
						break;
				}

				// If the item is right clicked, open the context menu
				if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
					ImGui::OpenPopup( item.Name.c_str(), ImGuiPopupFlags_::ImGuiPopupFlags_MouseButtonRight);


				if ( DrawContentItemContextMenu( item ) )
					break;
			}

			ImGui::EndTable();
		}

		ImGui::EndChild();
	}

	bool ContentBrowserPanel::DrawContentItemContextMenu( const ContentItem& a_Item )
	{
		if ( !ImGui::BeginPopup( a_Item.Name.c_str() ) )
			return false;

		ImGui::FunctionScope endPopup( &ImGui::EndPopup );

		if ( !a_Item.IsImported && a_Item.Type != EFileType::Folder )
		{
			if ( ImGui::MenuItem( "Import" ) )
			{
				AssetImporter::ImportAsset( m_CurrentDirectory / a_Item.Name );
				ReconstructFolderHierarchy();
				return true;
			}
		}

		if ( ImGui::MenuItem( "Open" ) )
		{
			if ( OnContentItemOpened( a_Item ) )
			{
				// If the item was opened, break the content items loop.
				return true;
			}
		}

		if ( ImGui::MenuItem( "Rename" ) )
		{

		}

		// Delete Option
		{
			ImGui::Separator();
			ImGui::ScopedStyleCol textColor( ImGuiCol_Text, ImVec4(Editor::Style::Colors::Red) );
			if ( ImGui::MenuItem( "Delete" ) )
			{
				IO::FilePath filePath = m_CurrentDirectory / a_Item.Name;
				if ( filePath.Remove() )
				{
					TE_CORE_INFO( "Deleted file: {0}", filePath.ToString() );
					ReconstructFolderHierarchy();
					return true;
				}
				else
				{
					TE_CORE_ERROR( "Failed to delete file: {0}", filePath.ToString() );
				}
			}
		}
		return false;
	}

	void ContentBrowserPanel::DrawContentBrowserSettings()
	{
		if ( !ImGui::BeginPopup( "Content Browser Settings" ) )
			return;

		ImGui::SliderFloat( "Thumbnail Size", &m_ContentThumbnailSize, 64.0f, 256.0f );
		ImGui::EndPopup();
	}

	bool ContentBrowserPanel::OnContentItemOpened( const ContentItem& a_Item )
	{
		switch ( a_Item.Type )
		{
		case EFileType::Folder:
		{
			OpenFolder( m_CurrentDirectory / a_Item.Name );
			return true;
		}
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	bool ContentItem::OnImGuiDraw( const ImVec2& a_Size ) const 
	{
		// Create an Image Button with the file name at the bottom
		ImGui::BeginGroup();
		{
			float paddingX = 25.f;
			float paddingY = 25.f;
			paddingX = a_Size.x < paddingX ? 0 : paddingX;
			paddingY = a_Size.y < paddingY ? 0 : paddingY;

			SharedPtr<Texture> icon = nullptr;
			if ( IsImported || !IsAsset() )
				icon = ContentItemIcons::s_FileTypeIcons[Type];
			else // Must be an Unimported Asset
				icon = ContentItemIcons::s_UnimportedAssetIcon;

			// Draw the thumbnail
			{
				ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_FramePadding, { paddingX, paddingY } );
				ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 1.f, 1.f, 1.f, 1.f ) );
				ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 1.f, 1.f, 1.f, 0.5f ) );

				ImGui::ImageButton(
					Name.c_str(),
					(ImTextureID)icon->GetRendererID(),
					{ a_Size.x - paddingX * 2, a_Size.y - paddingY * 2 },
					{ 0,1 }, { 1,0 } );

				ImGui::PopStyleVar();
				ImGui::PopStyleColor( 3 );
			}

			if ( ImGui::BeginDragDropSource() )
			{
				std::string filePath = ( Owner.GetDirectory() / Name ).ToString();
				ImGui::SetDragDropPayload( TE_PAYLOAD_ASSET_HANDLE, &Handle, sizeof(AssetHandle) );
				ImGui::Image( 
					(ImTextureID)icon->GetRendererID(),
					{ a_Size.x - paddingX * 2, a_Size.y - paddingY * 2 },
					{ 0,1 }, { 1,0 } );

				ImGui::PushTextWrapPos( a_Size.x - paddingX * 2 );
				ImGui::Text( Name.c_str() );
				ImGui::PopTextWrapPos();

				ImGui::EndDragDropSource();
			}

			// Draw the file name
			ImGui::PushTextWrapPos( ImGui::GetCursorPosX() + a_Size.x );
			ImGui::Text( Name.c_str() );
			ImGui::PopTextWrapPos();
		}
		ImGui::EndGroup();

		// If the item is double clicked, open it
		const bool wasOpened = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left );
		return wasOpened;
	}
}

#endif // IS_EDITOR