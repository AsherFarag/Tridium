#include "tripch.h"
#ifdef IS_EDITOR

#include "ContentBrowser.h"

#include "imgui.h"

namespace Tridium::Editor {

	void ContentBrowser::OnImGuiDraw()
	{
		m_CurrentDirectory = "Content";

		if ( !ImGui::Begin( m_Name.c_str() ) )
		{
			ImGui::End();
			return;
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)( panelWidth / cellSize );
		if ( columnCount < 1 )
			columnCount = 1;

		ImGui::Columns( columnCount, 0, false );

		for ( auto& directoryEntry : std::filesystem::directory_iterator( m_CurrentDirectory ) )
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();

			if ( directoryEntry.is_directory() )
				ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 1, 0, 0, 1 ) );

			if ( ImGui::Button( filenameString.c_str(), { thumbnailSize, thumbnailSize } ) )
			{
				m_CurrentDirectory = directoryEntry.path().string();
				break;
			}

			if ( directoryEntry.is_directory() )
				ImGui::PopStyleColor();
		}

		ImGui::End();
	}

}

#endif // IS_EDITOR