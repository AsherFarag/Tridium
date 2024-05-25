#include "tripch.h"
#ifdef IS_EDITOR
#include "ScriptEditor.h"

#include <fstream>
#include <sstream>

namespace Tridium::Editor {

	static bool DisplayContents( ScriptTextFile& file )
	{
		ImGuiTabItemFlags tabFlags = file.Modified ? ImGuiTabItemFlags_UnsavedDocument : ImGuiTabItemFlags_None;
		bool isOpen = true;
		if ( !ImGui::BeginTabItem( file.GetFileName().c_str(), &isOpen, tabFlags ) )
			return isOpen;

		if ( ImGui::IsItemHovered() )
		{
			ImGui::BeginTooltip();
			ImGui::Text( file.GetFilePath().c_str() );
			ImGui::EndTooltip();
		}

		ImGuiInputTextFlags textInputFlags = ImGuiInputTextFlags_AllowTabInput;

		ImVec2 textBoxSize = ImGui::GetContentRegionAvail();
		textBoxSize.y -= ImGui::GetTextLineHeight() * 1.75;

		// Display the text editor
		bool wasModified = ImGui::InputTextMultiline( "##source",
			&file.GetContent()[0],
			file.GetContent().capacity(),
			textBoxSize,
			textInputFlags );

		if ( wasModified )
			file.Modified = true;

		if ( ImGui::Button( "Save" ) )
			file.SaveFile();

		ImGui::EndTabItem();

		return isOpen;
	}

	void ScriptEditor::OnImGuiDraw()
	{
		if ( !ImGui::Begin( "Script Editor", &m_Open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse) )
			return;

		static bool newFilePopUp = false;
		static bool openFilePopUp = false;

		if ( ImGui::BeginMenuBar() )
		{
			if (ImGui::BeginMenu( "File" ))
			{
				if ( ImGui::MenuItem( "New" ) ) newFilePopUp = true;
				if ( ImGui::MenuItem( "Open" ) ) openFilePopUp = true;

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		#pragma region New File Pop Up
		
		if ( newFilePopUp )
			ImGui::OpenPopup( "NewFile" );

		if ( ImGui::BeginPopupModal( "NewFile" ) )
		{
			static char filePath[ 1024 ] = { "Content/Scripts/Component.lua" };
			ImGui::InputText( "File Path", filePath, 1024 );

			ImGui::SameLine();
			if ( ImGui::Button( "Open" ) )
			{
				newFilePopUp = false;

				// Make a new file
				std::ofstream newFile( filePath, std::ios::out | std::ios::app );
				newFile.close();

				if ( m_ScriptTextFiles.emplace_back().LoadFile( filePath ) == false )
					m_ScriptTextFiles.pop_back();

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if ( ImGui::Button( "Cancel" ) )
			{
				newFilePopUp = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		#pragma endregion


		#pragma region Open File Pop Up

		if ( openFilePopUp )
			ImGui::OpenPopup( "OpenFile" );

		if ( ImGui::BeginPopupModal( "OpenFile" ) )
		{
			static char filePath[ 1024 ] = { "Content/Scripts/Component.lua" };
			ImGui::InputText( "File Path", filePath, 1024 );

			ImGui::SameLine();
			if ( ImGui::Button( "Open" ) )
			{
				openFilePopUp = false;
				if ( m_ScriptTextFiles.emplace_back().LoadFile( filePath ) == false )
					m_ScriptTextFiles.pop_back();

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if ( ImGui::Button( "Cancel" ) )
			{
				openFilePopUp = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		#pragma endregion

		#pragma region Draw Open Files

		if ( ImGui::BeginTabBar("Script File Tabs") )
		{
			static int fileToClose = -1;
			for ( int i = m_ScriptTextFiles.size() - 1; i >= 0; --i )
			{
				auto& file = m_ScriptTextFiles[ i ];
				if ( DisplayContents( file ) )
					continue;

				// If the file has been modified and the user has requested to close this file,
				// open an 'Are you sure?' prompt. 
				if ( file.Modified )
				{
					ImGui::OpenPopup( "AreYouSure?" );
					fileToClose = i;
				}

				// Else, remove the file immediately
				else
				{
					m_ScriptTextFiles.erase( m_ScriptTextFiles.begin() + i );
				}
			}

			if ( ImGui::BeginPopupModal( "AreYouSure?", nullptr, ImGuiWindowFlags_NoResize ) )
			{
				ImGui::Text( "Are you sure you want to close this script without saving?" );

				const ImVec2 buttonSize = ImVec2( 30, 20 );

				ImGui::SetCursorPosX( ImGui::GetContentRegionAvail().x / 2 - buttonSize.x + 5 );

				if ( ImGui::Button( "Yes", buttonSize ) )
				{
					m_ScriptTextFiles.erase( m_ScriptTextFiles.begin() + fileToClose );
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if ( ImGui::Button( "No", buttonSize ) )
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::EndTabBar();
		}

		#pragma endregion

		ImGui::End();

		if ( m_Open == false )
			Close();
	}


	ScriptTextFile::ScriptTextFile( const std::string& a_FilePath )
	{
		LoadFile( a_FilePath );
	}

	bool ScriptTextFile::LoadFile( const std::string& a_FilePath )
	{
		// Open a read file stream
		std::fstream file( a_FilePath, std::ios::in );
		if ( !file.is_open() )
		{
			TE_CORE_ERROR( "Failed to load file at: {0}", a_FilePath );
			return false;
		}

		m_DirectoryPath = GetDirectoryPath( a_FilePath );
		m_FileName = GetFileName( a_FilePath );

		std::stringstream buffer;
		buffer << file.rdbuf();
		m_Content = buffer.str();
		file.close();

		if ( !file )
			TE_CORE_ERROR( "Fail occured when closing file [{0}]", a_FilePath );

		if ( m_Content.capacity() < 1024 * 16 )
			m_Content.reserve( 1024 * 16 );

		return true;
	}

	bool ScriptTextFile::SaveFile( const std::string& a_FilePath )
	{
		// Check if this file needs to be saved
		if ( GetFilePath() == a_FilePath && !Modified )
			return false;

		std::fstream file( a_FilePath, std::ios::out );
		if ( !file ) 
		{
			TE_CORE_WARN( "Could not open [{0}] for writing!", a_FilePath );
			return false;
		}

		// Write the contents to the file
		file << m_Content.c_str();
		file.close();
		Modified = false;

		if ( !file )
			TE_CORE_ERROR( "Fail occured when closing file [{0}]", a_FilePath );

		return true;
	}

	std::string ScriptTextFile::GetDirectoryPath( const std::string& a_FilePath )
	{
		size_t pos = a_FilePath.find_last_of( "/\\" );
		if ( pos != std::string::npos ) {
			return a_FilePath.substr( 0, pos );
		}
		TE_CORE_WARN( "[{0}] is not a file directory path!", a_FilePath );
		return "";  // or return ".", or another default value if there's no directory part
	}

	std::string ScriptTextFile::GetFileName( const std::string& a_FilePath )
	{
		size_t pos = a_FilePath.find_last_of( "/\\" );
		if ( pos != std::string::npos ) {
			return a_FilePath.substr( pos + 1 );
		}
		TE_CORE_WARN( "[{0}] does not contain a file name!", a_FilePath );
		return a_FilePath;  // or return "" if no file name found
	}

}
#endif // IS_EDITOR