#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorUtil.h"
#include "Editor.h"
#include <Tridium/Core/Application.h>


namespace Tridium::Editor::Util {


	FileDialogLayer::FileDialogLayer( FileDialogCallback a_Callback, const std::string& a_DefaultPath )
		: Layer( "SaveFileDialogLayer" )
		, m_Callback( a_Callback )
		, m_FilePath( a_DefaultPath )
	{
	}

	void SaveFileDialogLayer::OnImGuiDraw()
	{
		ImGui::OpenPopup( "Save File" );

		ImGui::SetNextWindowPos( ImVec2( 0.5f * ImGui::GetIO().DisplaySize.x, 0.5f * ImGui::GetIO().DisplaySize.y ), ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );

		if ( !ImGui::BeginPopupModal( "Save File", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize ) )
			return;

		ImGui::InputTextWithHint( "##FilePath", "File Path", &m_FilePath );

		ImVec2 buttonSize = ImGui::CalcTextSize( "Cancel" );
		const ImVec2 buttonPadding = ImGui::GetStyle().FramePadding;
		buttonSize.x += buttonPadding.x * 2.0f;
		buttonSize.y += buttonPadding.y * 2.0f;

		bool shouldClose = false;

		ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1.0f );

		if ( ImGui::Button( "Save", buttonSize ) )
		{
			shouldClose = true;
			ImGui::CloseCurrentPopup();
			m_Callback( m_FilePath );
		}

		ImGui::SameLine();

		if ( ImGui::Button( "Cancel", buttonSize ) )
		{
			ImGui::CloseCurrentPopup();
			shouldClose = true;
		}

		ImGui::PopStyleVar();

		ImGui::EndPopup();

		if ( shouldClose )
		{
			Application::Get().PopOverlay( this, true );
		}
	}

	void LoadFileDialogLayer::OnImGuiDraw()
	{
		ImGui::OpenPopup( "Load File" );

		ImGui::SetNextWindowPos( ImVec2( 0.5f * ImGui::GetIO().DisplaySize.x, 0.5f * ImGui::GetIO().DisplaySize.y ), ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );

		if ( !ImGui::BeginPopupModal( "Load File", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize ) )
			return;

		ImGui::InputTextWithHint( "##FilePath", "File Path", &m_FilePath );

		ImVec2 buttonSize = ImGui::CalcTextSize( "Cancel" );
		const ImVec2 buttonPadding = ImGui::GetStyle().FramePadding;
		buttonSize.x += buttonPadding.x * 2.0f;
		buttonSize.y += buttonPadding.y * 2.0f;

		bool shouldClose = false;

		ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1.0f );

		if ( ImGui::Button( "Load", buttonSize ) )
		{
			shouldClose = true;
			ImGui::CloseCurrentPopup();
			m_Callback( m_FilePath );
		}

		ImGui::SameLine();

		if ( ImGui::Button( "Cancel", buttonSize ) )
		{
			ImGui::CloseCurrentPopup();
			shouldClose = true;
		}

		ImGui::PopStyleVar();

		ImGui::EndPopup();

		if ( shouldClose )
		{
			Application::Get().PopOverlay( this, true );
		}
	}

	NewFileDialogLayer::NewFileDialogLayer( const std::string& a_FileTypeName, FileDialogCallback a_Callback, const std::string& a_DefaultPath )
		: FileDialogLayer( a_Callback, a_DefaultPath )
	{
		m_FileTypeName = a_FileTypeName;
	}

	void NewFileDialogLayer::OnImGuiDraw()
	{
		const std::string title = "New " + m_FileTypeName;
		ImGui::OpenPopup( title.c_str() );

		ImGui::SetNextWindowPos( ImVec2( 0.5f * ImGui::GetIO().DisplaySize.x, 0.5f * ImGui::GetIO().DisplaySize.y ), ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );

		if ( !ImGui::BeginPopupModal( title.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize) )
			return;

		ImGui::InputTextWithHint( "##FilePath", "File Path", &m_FilePath );

		ImVec2 buttonSize = ImGui::CalcTextSize( "Create" );
		const ImVec2 buttonPadding = ImGui::GetStyle().FramePadding;
		buttonSize.x += buttonPadding.x * 2.0f;
		buttonSize.y += buttonPadding.y * 2.0f;

		bool shouldClose = false;

		ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1.0f );

		if ( ImGui::Button( "Create", buttonSize ) )
		{
			shouldClose = true;
			ImGui::CloseCurrentPopup();
			m_Callback( m_FilePath );
		}

		ImGui::SameLine();

		if ( ImGui::Button( "Cancel", buttonSize ) )
		{
			ImGui::CloseCurrentPopup();
			shouldClose = true;
		}

		ImGui::PopStyleVar();

		ImGui::EndPopup();

		if ( shouldClose )
		{
			Application::Get().PopOverlay( this, true );
		}
	}

};

#endif // IS_EDITOR