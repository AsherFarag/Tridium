#include "tripch.h"
#ifdef IS_EDITOR
#include "ScriptEditorPanel.h"
#include <Tridium/Core/Application.h>

#include <fstream>
#include <sstream>

namespace Tridium::Editor {

#pragma region Script Editor

	void ScriptEditorPanel::OnImGuiDraw()
	{
		ImGuiBegin( ImGuiWindowFlags_MenuBar );

		m_IsFocused = ImGui::IsWindowFocused();

		static bool newFilePopUp = false;
		static bool openFilePopUp = false;

		if ( ImGui::BeginMenuBar() )
		{
			if (ImGui::BeginMenu( "File" ))
			{
				if ( ImGui::MenuItem( "New" ) ) newFilePopUp = true;
				if ( ImGui::MenuItem( "Open" ) ) openFilePopUp = true;

				ImGui::Separator();

				if ( ImGui::MenuItem( "Save", "Ctrl + S" ) ) SaveCurrentFile();
				if ( ImGui::MenuItem( "Save All", "Ctrl + Alt + S" ) ) SaveAllFiles();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		#pragma region New File Pop Up
		
		if ( newFilePopUp )
			ImGui::OpenPopup( "NewFile" );

		if ( ImGui::BeginPopupModal( "NewFile" ) )
		{
			static char filePath[ 1024 ] = { "Content/Scripts/ComponentTemplate.lua" };
			ImGui::InputText( "File Path", filePath, 1024 );

			ImGui::SameLine();
			if ( ImGui::Button( "New" ) )
			{
				newFilePopUp = false;

				std::ifstream existingFile( filePath );
				// If there is no file at the filePath,
				// make a new file.
				if ( !existingFile )
				{
					std::ofstream newFile( filePath, std::ios::out | std::ios::app );
					// Create a new file and write in a component template
					newFile << "function OnBeginPlay()\nend\n\nfunction OnUpdate( deltaTime )\nend\n\nfunction OnDestroy()\nend\n"; 
					newFile.close();
				}
				existingFile.close();

				OpenFile( filePath );

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
			static char filePath[ 1024 ] = ".lua";
			ImGui::InputText( "File Path", filePath, 1024 );

			ImGui::SameLine();
			if ( ImGui::Button( "Open" ) )
			{
				openFilePopUp = false;
				OpenFile( filePath );

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

		if ( ImGui::BeginTabBar("Script File Tabs", ImGuiTabBarFlags_AutoSelectNewTabs ) )
		{
			static int fileToClose = -1;
			for ( int i = m_ScriptTextFiles.size() - 1; i >= 0; --i )
			{
				auto& file = m_ScriptTextFiles[ i ];
				if ( DisplayFileContents( file ) )
					continue;

				// If the file has been modified and the user has requested to close this file,
				// open an 'Are you sure?' prompt. 
				if ( file.Modified )
				{
					ImGui::OpenPopup( "Are You Sure? ##CloseTab" );
					fileToClose = i;
				}

				// Else, remove the file immediately
				else
				{
					CloseFile( i );
				}
			}

			if ( ImGui::BeginPopupModal( "Are You Sure? ##CloseTab", nullptr, ImGuiWindowFlags_NoResize ) )
			{
				ImGui::Text( "Are you sure you want to close this script without saving?" );

				const ImVec2 buttonSize = ImVec2( 30, 20 );

				ImGui::SetCursorPosX( ImGui::GetContentRegionAvail().x / 2 - buttonSize.x + 5 );

				if ( ImGui::Button( "Yes", buttonSize ) )
				{
					CloseFile( fileToClose );
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

		if ( !m_Open )
		{
			bool isAFileModified = false;
			for ( auto& file : m_ScriptTextFiles )
				if ( file.Modified ) isAFileModified = true;

			if ( !isAFileModified )
			{
				Close();
				return;
			}

			m_Open = true;
			ImGui::OpenPopup( "Are You Sure? ##CloseWindow" );
		}

		if ( ImGui::BeginPopupModal( "Are You Sure? ##CloseWindow", nullptr, ImGuiWindowFlags_NoResize ) )
		{
			ImGui::Text( "Are you sure you want to close without saving?" );

			const ImVec2 buttonSize = ImVec2( 30, 20 );

			ImGui::SetCursorPosX( ImGui::GetContentRegionAvail().x / 2 - buttonSize.x + 5 );

			if ( ImGui::Button( "Yes", buttonSize ) )
			{
				Close();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if ( ImGui::Button( "No", buttonSize ) )
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	bool ScriptEditorPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		bool control = Input::IsKeyPressed( Input::KEY_LEFT_CONTROL );
		bool alt = Input::IsKeyPressed( Input::KEY_LEFT_ALT );

		switch ( e.GetKeyCode() )
		{
			case Input::KEY_S:
			{
				if ( control && alt )
				{
					SaveAllFiles();
					return true;
				}
				if ( control )
				{
					SaveCurrentFile();
					return true;
				}
				break;
			}
		}

		return false;
	}

	bool ScriptEditorPanel::DisplayFileContents( ScriptTextFile& file )
	{
		ImGuiTabItemFlags tabFlags = ImGuiTabItemFlags_None;
		tabFlags |= file.Modified ? ImGuiTabItemFlags_UnsavedDocument : 0;

		bool isOpen = true;
		if ( !ImGui::BeginTabItem( ( file.GetFileName() + "##" + file.GetPath().ToString() ).c_str(), &isOpen, tabFlags) )
			return isOpen;

		m_CurrentTextFile = &file;

		if ( ImGui::IsItemHovered() )
		{
			ImGui::BeginTooltip();
			ImGui::Text( file.GetPath().ToString().c_str());
			ImGui::EndTooltip();
		}

		ImGuiInputTextFlags textInputFlags = ImGuiInputTextFlags_AllowTabInput;

		ImVec2 textBoxSize = ImGui::GetContentRegionAvail();
		textBoxSize.y -= ImGui::GetTextLineHeight() * 1.75;

			// Display the text editor
		file.Modified |= ImGui::InputTextMultiline("##source",
			&file.GetContent()[0],
			file.GetContent().capacity(),
			textBoxSize,
			textInputFlags);

		ImGui::EndTabItem();

		return isOpen;
	}

	void ScriptEditorPanel::SaveAllFiles()
	{
		for ( auto& file : m_ScriptTextFiles )
		{
			file.SaveFile();
		}
	}

	void ScriptEditorPanel::OpenFile( const  IO::FilePath& a_FilePath )
	{
		// Ensure this file isn't already open
		for ( auto& file : m_ScriptTextFiles )
		{
			if ( file.GetPath() == a_FilePath )
				return;
		}

		bool success = m_ScriptTextFiles.emplace_back().LoadFile( a_FilePath );

		if ( !success )
			m_ScriptTextFiles.pop_back();
	}

	void ScriptEditorPanel::SaveCurrentFile()
	{
		if ( m_CurrentTextFile )
			m_CurrentTextFile->SaveFile();
	}

	void ScriptEditorPanel::CloseFile( uint32_t index )
	{
		if ( index >= m_ScriptTextFiles.size() )
			return;

		if ( &m_ScriptTextFiles[ index ] == m_CurrentTextFile )
			m_CurrentTextFile == nullptr;

		m_ScriptTextFiles.erase( m_ScriptTextFiles.begin() + index );
	}

#pragma endregion

#pragma region File

	ScriptTextFile::ScriptTextFile( const IO::FilePath& a_FilePath )
	{
		LoadFile( a_FilePath );
	}

	bool ScriptTextFile::LoadFile( const  IO::FilePath& a_FilePath )
	{
		// Open a read file stream
		std::fstream file( a_FilePath.ToString(), std::ios::in);
		if ( !file.is_open() )
		{
			TE_CORE_ERROR( "Failed to load file at: {0}", a_FilePath.ToString() );
			return false;
		}

		m_Path = a_FilePath;

		std::stringstream buffer;
		buffer << file.rdbuf();
		m_Content = buffer.str();
		file.close();

		if ( !file )
			TE_CORE_ERROR( "Fail occured when closing file [{0}]", a_FilePath.ToString() );

		if ( m_Content.capacity() < 1024 * 16 )
			m_Content.reserve( 1024 * 16 );

		return true;
	}

	bool ScriptTextFile::SaveFile( const  IO::FilePath& a_FilePath )
	{
		// Check if this file needs to be saved
		if ( GetPath() == a_FilePath && !Modified )
			return false;

		std::fstream file( a_FilePath.ToString(), std::ios::out);
		if ( !file ) 
		{
			TE_CORE_WARN( "Could not open [{0}] for writing!", a_FilePath.ToString() );
			return false;
		}

		// Write the contents to the file
		file << m_Content.c_str();
		file.close();
		Modified = false;

		if ( !file )
			TE_CORE_ERROR( "Fail occured when closing file [{0}]", a_FilePath.ToString() );

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

#pragma endregion

}
#endif // IS_EDITOR