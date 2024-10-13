#pragma once
#ifdef IS_EDITOR
#include <Tridium/ImGui/ImGui.h>
#include <Tridium/Core/Application.h>

namespace Tridium::Editor::Util {

	template <typename SaveFileDialogCallback>
	class SaveFileDialogLayer : public Layer
	{
	public:
		SaveFileDialogLayer( SaveFileDialogCallback a_Callback, const std::string& a_DefaultPath )
			: Layer( "SaveFileDialogLayer" )
			, m_Callback( a_Callback )
			, m_FilePath( a_DefaultPath )
		{
		}

		virtual void OnImGuiDraw() override
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

	private:
		SaveFileDialogCallback m_Callback;
		std::string m_FilePath;
	};


	template <typename SaveFileDialogCallback>
	void OpenSaveFileDialog( const std::string& a_DefaultFilePath, SaveFileDialogCallback a_Callback )
	{
		Application::Get().PushOverlay( new SaveFileDialogLayer( a_Callback, a_DefaultFilePath ) );
	}

	template <typename LoadFileDialogCallback>
	class LoadFileDialogLayer : public Layer
	{
	public:
		LoadFileDialogLayer( LoadFileDialogCallback a_Callback, const std::string& a_DefaultPath )
			: Layer( "LoadFileDialogLayer" )
			, m_Callback( a_Callback )
			, m_FilePath( a_DefaultPath )
		{
		}

		virtual void OnImGuiDraw() override
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

	private:
		LoadFileDialogCallback m_Callback;
		std::string m_FilePath;
	};

	template <typename LoadFileDialogCallback>
	void OpenLoadFileDialog( const std::string& a_DefaultFilePath, LoadFileDialogCallback a_Callback )
	{
		Application::Get().PushOverlay( new LoadFileDialogLayer( a_Callback, a_DefaultFilePath ) );
	}
};

#endif // IS_EDITOR