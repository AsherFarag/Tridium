#include "AssetBrowserPanel.h"

#if WITH_EDITOR

#include <Tridium/IO/FilePath.h>

namespace Tridium {

	void AssetBrowserPanel::OnEvent( Event& a_Event )
	{
	}

	void AssetBrowserPanel::OnUpdate( float a_DeltaTime )
	{
	}

	void AssetBrowserPanel::OnDraw( StringView a_Name, bool& o_Open )
	{
		if ( ImGui::Begin( a_Name.data(), &o_Open ) )
		{
			ImGui::Columns( 2, "Content Browser Columns", true );

			// ImGui does not serialize the column width, so we need to set it once on the first frame.
			static bool s_SetColumnWidth = []() -> bool
			{
				ImGui::SetColumnWidth( 0, 250.0f );
				return true;
			}( );

			UI_DrawFolderHierarchy();

			ImGui::NextColumn();

			UI_DrawAssetBrowserHeader();

			UI_DrawAssetItems();
		}

		ImGui::End();
	}

	void AssetBrowserPanel::UI_DrawFolderHierarchy()
	{
		if ( ImGui::BeginChild( "Folder Hierarchy", { 0, 0 }, ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar ) )
		{
			// Add some padding to the top of the window
			ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 2.0f );

			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Framed;

			if ( ImGui::TreeNodeEx( TE_ICON_FOLDER " Dummy Folder", treeNodeFlags ) )
			{
				ImGui::TreePop();
			}
		}
		ImGui::EndChild();
	}

	void AssetBrowserPanel::UI_DrawAssetBrowserHeader()
	{
		// Get the cursor position and frame padding for the input text box
		const ImVec2& framePadding = ImGui::GetStyle().FramePadding;
		const ImVec2 cursorPos = ImGui::GetCursorScreenPos() + framePadding;

		//  - Draw the directory path input box - 
		// If the directory input text box is active, set the text color to the default text color
		// Otherwise, set the text color to invisible.
		{
			ImGui::ScopedStyleCol TextColour( ImGuiCol_Text, m_Browser.DirectoryInputActive
											 ? ImGui::GetStyleColorVec4( ImGuiCol_Text )
											 : ImVec4() );

			// Allow the following folder button to overlap the input text box
			ImGui::SetNextItemAllowOverlap();

			if ( ImGui::InputText( "##Directory Path", &m_Browser.CurrentDirectory, ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
			}

			m_Browser.DirectoryInputActive = ImGui::IsItemActive();
		}

		// Draw Refresh Button
		ImGui::SameLine();
		if ( ImGui::Button( TE_ICON_ROTATE ) )
		{
			//LOG( Log, Info, "Refreshed content browser" );
		}

		// Draw Seperator
		ImGui::SameLine();
		ImGui::Text( " | " );

		// Draw Content Search Filter
		{
			static thread_local char TempBuffer[256] = "";
			static_assert( sizeof( TempBuffer ) == sizeof( m_Browser.SearchFilter.InputBuf ),
						   "Update TempBuffer size to ImGuiTextFilter.InputBuf size" );

			ImGui::SameLine();
			ImGui::Text( TE_ICON_MAGNIFYING_GLASS );

			ImGui::SameLine();
			const float Width = ImGui::GetContentRegionAvail().x
				- ImGui::CalcTextSize( TE_ICON_GEAR ).x
				- ImGui::GetStyle().WindowPadding.x * 2
				- 5.0f;

			ImGui::SetNextItemWidth( Width );
			ImGui::SetNextItemAllowOverlap();
			ImGui::InputText( "##Content Search Filter", TempBuffer, sizeof( TempBuffer ) );

			//float buttonWidth = ImGui::CalcTextSize( ICON_FA_X ).x + ImGui::GetStyle().FramePadding.x * 2;
			//ImGui::SameLine( );
			//if ( ImGui::Button( ICON_FA_X ) )
			//	m_ContentSearchFilter.clear();
		}


		// Draw the settings button
		{
			ImGui::SameLine( ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize( TE_ICON_GEAR ).x - ImGui::GetStyle().WindowPadding.x );
			if ( ImGui::Button( TE_ICON_GEAR ) )
				ImGui::OpenPopup( "Content Browser Settings" );
		}

		const ImVec2 resetCursorPos = ImGui::GetCursorScreenPos();

		// Draw the directory path buttons
		if ( !m_Browser.DirectoryInputActive )
		{
			ImGui::SetCursorScreenPos( cursorPos ); // Reset the cursor position

			ImGui::Text( " " TE_ICON_FOLDER_OPEN " " );
			ImGui::SameLine();

			ImGui::ScopedStyleVar frame( ImGuiStyleVar_FrameBorderSize, 0.f );
			ImGui::ScopedStyleCol buttonColor( ImGuiCol_Button, ImVec4() );

			FilePath currentDirectory = m_Browser.CurrentDirectory;
			Array<String> folders; //folders.Reserve( currentDirectory.count() );

			//for ( const StringView token : currentDirectory )
			//{
			//	Folders.emplace_back( token );
			//}


			// Collect all the folders in the current directory path
			//{
			//	AssetAddress TempPath = CurrentDirectory;
			//	while ( !TempPath.IsEmpty() )
			//	{
			//		Folders.emplace_front( TempPath.GetFilename().ToString() );
			//		TempPath = TempPath.GetParentPath();
			//	}
			//}

			uint32_t i = 0;

			for ( const String& folder : folders )
			{
				ImGui::ScopedID id( i );

				if ( ImGui::SmallButton( folder.c_str() ) )
				{
					for ( size_t j = folders.Size() - 1; j > i; --j )
						currentDirectory = currentDirectory.GetParentPath();

					//TODO( Asher, "OpenFolder(m_CurrentDirectory) " );
					m_Browser.CurrentDirectory = currentDirectory.ToString();

					break;
				}

				if ( i < folders.Size() - 1 )
				{
					ImGui::SameLine();
					ImGui::TextUnformatted( ">" );
					ImGui::SameLine();
				}

				i++;
			}
		}

		ImGui::SetCursorScreenPos( resetCursorPos ); // Reset the cursor position
	}

	void AssetBrowserPanel::UI_DrawAssetItems()
	{
		if ( ImGui::BeginChild( "Asset Items", { 0, 0 } ) )
		{
		}
		ImGui::EndChild();
	}

} // namespace Tridium

#endif // WITH_EDITOR