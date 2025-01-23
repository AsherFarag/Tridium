#include "tripch.h"
#if IS_EDITOR
#include "ContentBrowserPanel.h"

#include <Tridium/Rendering/Material.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>
#include <Tridium/Core/Application.h>
#include <Tridium/Asset/EditorAssetManager.h>

#include <Editor/Editor.h>
#include <Editor/AssetImporter.h>
#include <Editor/EditorUtil.h>
#include <Editor/Util/AssetInfo.h>
#include <Editor/EditorStyle.h>
#include "ScriptEditorPanel.h"
#include "Asset/MaterialEditorPanel.h"

#include "imgui_internal.h"
#include <thread>
#include <fstream>

namespace Tridium::Editor {

	std::unordered_map<EFileType, SharedPtr<Texture>> ContentItemIcons::s_FileTypeIcons = std::unordered_map<EFileType, SharedPtr<Texture>>();
	SharedPtr<Texture> ContentItemIcons::s_UnimportedAssetIcon = nullptr;

	const char* FileTypeToString( EFileType a_Type )
	{
		switch ( a_Type )
		{
		case EFileType::None:       return "None";
		case EFileType::Scene:      return "Scene";
		case EFileType::Material:   return "Material";
		case EFileType::MeshSource: return "Mesh Source";
		case EFileType::StaticMesh: return "Static Mesh";
		case EFileType::Shader:     return "Shader";
		case EFileType::Texture:    return "Texture";
		case EFileType::CubeMap:    return "Cube Map";
		case EFileType::LuaScript:        return "Lua";
		case EFileType::Folder:     return "Folder";
		}

		return "Unknown";

	}

	ContentBrowserPanel::ContentBrowserPanel() : Panel( TE_ICON_FOLDER " Content Browser" )
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
			{ EFileType::LuaScript,	     TextureLoader::LoadTexture( iconFolder / "file-code.png" ) },
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

		// If the empty space is right clicked, open the content browser context menu
		{
			if ( ImGui::IsWindowHovered( ImGuiHoveredFlags_ChildWindows ) && ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
				ImGui::OpenPopup( "Content Browser Context Menu" );

			if ( ImGui::BeginPopup( "Content Browser Context Menu" ) )
			{
				if ( ImGui::MenuItem( "Create Folder" ) )
				{
				}

				ImGui::Separator();

				if ( ImGui::BeginMenu( "Create Asset" ) )
				{
					if ( ImGui::MenuItem( "Material" ) )
					{
						Util::OpenNewFileDialog( "Material", "m_NewMaterial.tmat", [&](const std::string& a_FilePath)
							{
								SharedPtr<Material> material = MakeShared<Material>();

								AssetMetaData metaData =
								{
									.Handle = AssetHandle::Create(),
									.AssetType = EAssetType::Material,
									.Path = m_CurrentDirectory / a_FilePath,
									.Name = a_FilePath,
									.IsAssetLoaded = true,
								};

								EditorAssetManager::Get()->CreateAsset( metaData, material );
								AssetFactory::SaveAsset( metaData, material );
							} );
					}

					if ( ImGui::MenuItem( "Script" ) )
					{
						Util::OpenNewFileDialog( "Script", "m_NewScript.lua", [&]( const std::string& a_FilePath )
							{
								std::ofstream file( ( m_CurrentDirectory / a_FilePath ).ToString() );
								file.close();
							} );
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}
		}

		const float padding = 6.0f;
		const float cellSize = 100.0f * m_ContentThumbnailSize + padding;
		const float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)( panelWidth / cellSize );
		if ( columnCount < 1 )
			columnCount = 1;

		ImGui::ScopedStyleVar cellPadding( ImGuiStyleVar_CellPadding, { padding, padding } );
		if ( ImGui::BeginTable( "Folder Contents Items", columnCount ) )
		{
			ImGuiTextFilter filter( m_ContentSearchFilter.c_str() );

			for ( auto& item : m_FolderHeirarchy[m_CurrentDirectory] )
			{
				if ( !filter.PassFilter( item.Name.c_str() ) )
					continue;

				ImGui::TableNextColumn();
				const bool wasOpened = item.OnImGuiDraw( m_ContentThumbnailSize );

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

		ImGui::SliderFloat( "Thumbnail Size", &m_ContentThumbnailSize, 0.1f, 5.0f );
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
			case EFileType::Material:
			{
				SharedPtr<Material> material = AssetManager::GetAsset<Material>( a_Item.Handle );
				if ( material )
				{
					GetEditorLayer()->GetOrEmplacePanel<MaterialEditorPanel>()->SetMaterial( a_Item.Handle );
				}
				break;
			}
			case EFileType::LuaScript:
			{
				ScriptEditorPanel* panel = GetEditorLayer()->GetOrEmplacePanel<ScriptEditorPanel>();
				panel->OpenFile( m_CurrentDirectory / a_Item.Name );
				panel->Focus();
				break;
			}
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	//ImVec2 operator+( const ImVec2& a, const ImVec2& b )
	//{
	//	return ImVec2( a.x + b.x, a.y + b.y );
	//}

	//ImVec2 operator-( const ImVec2& a, const ImVec2& b )
	//{
	//	return ImVec2( a.x - b.x, a.y - b.y );
	//}

	//ImVec2 operator*( const ImVec2& a, const ImVec2& b )
	//{
	//	return ImVec2( a.x * b.x, a.y * b.y );
	//}

	//ImVec2 operator*( const ImVec2& a, float b )
	//{
	//	return ImVec2( a.x * b, a.y * b );
	//}

	bool RenderContentBrowserThumbnail( const char* a_FileName, ImTextureID a_ThumbnailIcon, const char* a_AssetType, const ImVec4& a_Color, float a_SizeMultiplier = 1.0f )
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if ( window->SkipItems )
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID( a_FileName );

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImVec2( 100.0f, 150.0f ) * a_SizeMultiplier;

		const ImRect bb( pos, pos + size );
		const ImVec2 innerPadding = ImVec2( 6.0f, 6.0f );
		const ImRect bbInner( bb.Min + innerPadding, bb.Max - innerPadding );
		const ImRect bbIcon( bbInner.Min, bbInner.Min + ImVec2( bbInner.GetWidth(), bbInner.GetWidth() ) );

		ImGui::ItemSize( bb, 0.0f );
		if ( !ImGui::ItemAdd( bb, id ) )
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, ImGuiButtonFlags_None );
		const ImU32 col = ImGui::GetColorU32( ( held && hovered ) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_WindowBg );

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Draw thumbnail Background
		drawList->AddRectFilled( bb.Min, bb.Max, col, 5.0f, ImDrawFlags_RoundCornersAll );
		// Draw thumbnail Icon
		drawList->AddImageRounded( a_ThumbnailIcon, bbIcon.Min, bbIcon.Max, ImVec2( 0, 1 ), ImVec2( 1, 0 ), ImGui::GetColorU32( ImVec4( 1, 1, 1, 1 ) ), 2.5f );

		// Draw color rect padding under the icon
		constexpr float colorDummyThickness = 2.0f;
		const ImRect bbColorDummyRect( 
			ImVec2( bb.Min.x, bbIcon.Max.y ),
			ImVec2( bb.Max.x, bbIcon.Max.y + colorDummyThickness ) );
		drawList->AddRectFilled( bbColorDummyRect.Min, bbColorDummyRect.Max, ImGui::GetColorU32( a_Color ) );

		// Draw file name
		const float fontSize = 16.0f * a_SizeMultiplier;
		const ImVec4 bbText( bbInner.Min.x, bbIcon.Max.y + 2.0f, bbInner.Max.x, bbIcon.Max.y + fontSize + 4.0f );
		drawList->AddText(
			ImGui::GetLightFont(), fontSize,
			ImVec2( bbInner.Min.x, bbIcon.Max.y + 2.0f + colorDummyThickness ),
			ImGui::GetColorU32( ImVec4( 1, 1, 1, 0.9f ) ),
			a_FileName,
			nullptr, bbInner.Max.x, &bbText );

		// Draw asset type at the bottom right corner
		const float assetTypeFontSize = 14.0f * a_SizeMultiplier;
		const ImVec4 bbAssetType( bbInner.Min.x, bbInner.Max.y - assetTypeFontSize - 2.0f, bbInner.Max.x, bbInner.Max.y );
		drawList->AddText(
			ImGui::GetLightFont(), assetTypeFontSize,
			ImVec2( bbInner.Min.x, bbInner.Max.y - assetTypeFontSize - 2.0f ),
			ImGui::GetColorU32( ImVec4( 1, 1, 1, 0.5f ) ),
			a_AssetType,
			nullptr, bbInner.Max.x, &bbAssetType );

		return pressed;
	}

	bool ContentItem::OnImGuiDraw( float a_Size ) const 
	{
		SharedPtr<Texture> icon = nullptr;
		if ( IsImported || !IsAsset() )
			icon = ContentItemIcons::s_FileTypeIcons[Type];
		else // Must be an Unimported Asset
			icon = ContentItemIcons::s_UnimportedAssetIcon;

		const AssetTypeInfo& typeInfo = AssetTypeManager::GetAssetTypeInfo( static_cast<EAssetType>( Type ) );
		ImVec4 color = ImVec4( typeInfo.Color.x, typeInfo.Color.y, typeInfo.Color.z, typeInfo.Color.w );

		RenderContentBrowserThumbnail( Name.c_str(), (ImTextureID)icon->GetRendererID(), FileTypeToString( Type ), color, a_Size );
		// If the item is double clicked, open it
		const bool wasOpened = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left );

		if ( ImGui::BeginItemTooltip() )
		{
			ImGui::Text( Name.c_str() );
			ImGui::EndTooltip();
		}

		if ( ImGui::BeginDragDropSource() )
		{
			std::string filePath = ( Owner.GetDirectory() / Name ).ToString();
			ImGui::SetDragDropPayload( TE_PAYLOAD_ASSET_HANDLE, &Handle, sizeof( AssetHandle ) );

			RenderContentBrowserThumbnail( Name.c_str(), (ImTextureID)icon->GetRendererID(), FileTypeToString( Type ), color, a_Size );

			ImGui::EndDragDropSource();
		}

		return wasOpened;
	}
}

#endif // IS_EDITOR