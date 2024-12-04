#include "tripch.h"
#if IS_EDITOR
#include "ScriptEditorPanel.h"
#include <Tridium/Core/Application.h>
#include <Editor/EditorUtil.h>
#include <Tridium/Reflection/Reflection.h>

#include <fstream>
#include <sstream>

namespace Tridium::Editor {

	ScriptEditorPanel::ScriptEditorPanel()
		: Panel( "Script Editor" )
	{
		SetTheme( ETheme::Dark );

		m_LuaLanguageDefinition = TextEditor::LanguageDefinition::Lua();

		for ( const auto&& [id, type] : Refl::ResolveMetaTypes() )
		{
			Refl::MetaType metaType = type;
			if ( HasFlag( metaType.GetClassFlags(), Refl::EClassFlags::Scriptable ) )
			{
				TextEditor::Identifier id;
				id.mDeclaration = "Tridium Type";
				m_LuaLanguageDefinition.mIdentifiers.emplace( std::string( metaType.GetCleanTypeName() ), id );
			}
		}
	}

	void ScriptEditorPanel::OnImGuiDraw()
	{
		OpenedScript* openedScript = GetOpenedScript( m_CurrentOpenedFile );

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar;
		if ( openedScript )
			windowFlags |= openedScript->Editor.IsTextChanged() ? ImGuiWindowFlags_UnsavedDocument : 0;

		if ( !ImGuiBegin( windowFlags ) )
		{
			ImGuiEnd();
			return;
		}

		DrawMenuBar();

		const float openedScriptTabsSizeY = ImGui::GetTextLineHeightWithSpacing();

		if ( openedScript )
		{
			ImVec2 size = ImGui::GetContentRegionAvail();
			size.y -= openedScriptTabsSizeY;
			openedScript->Editor.Render( m_Name.c_str(), size );
		}

		// Draw opened scripts tabs
		DrawOpenedScripts();

		ImGuiEnd();
	}

	void ScriptEditorPanel::OpenFile( const IO::FilePath& a_FilePath )
	{
		std::ifstream file( a_FilePath.ToString() );
		if ( !file.is_open() )
		{
			TE_CORE_ERROR( "Failed to open file: {0}", a_FilePath.ToString() );
			return;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();

		m_CurrentOpenedFile = a_FilePath;
		m_OpenedScripts[a_FilePath].Path = a_FilePath;
		m_OpenedScripts[a_FilePath].Editor.SetText( buffer.str() );
		m_OpenedScripts[a_FilePath].Editor.SetLanguageDefinition( m_LuaLanguageDefinition );

		file.close();
	}

	bool ScriptEditorPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		return false;
	}

	OpenedScript* ScriptEditorPanel::GetOpenedScript( const IO::FilePath& a_FilePath )
	{
		auto it = m_OpenedScripts.find( a_FilePath );
		if ( it == m_OpenedScripts.end() )
		{
			TE_CORE_ERROR( "Failed to find opened script: {0}", a_FilePath.ToString() );
			return nullptr;
		}

		return &it->second;
	}

	void ScriptEditorPanel::DrawMenuBar()
	{
		if ( !ImGui::BeginMenuBar() )
			return;

		if ( ImGui::BeginMenu( "File" ) )
		{
			if ( ImGui::MenuItem( "Open" ) )
			{
				Util::OpenLoadFileDialog( Application::GetActiveProject()->GetAssetDirectory() / "NewLuaScript.lua", [this](const IO::FilePath& a_FilePath)
					{
						OpenFile( a_FilePath );
					} );
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Edit" ) )
		{
			ImGui::Separator();

			if ( ImGui::BeginMenu( "Theme" ) )
			{
				if ( ImGui::MenuItem( "Light" ) )
				{
					SetTheme( ETheme::Light );
				}
				if ( ImGui::MenuItem( "Dark" ) )
				{
					SetTheme( ETheme::Dark );
				}
				if ( ImGui::MenuItem( "Retro Blue" ) )
				{
					SetTheme( ETheme::BlueRetro );
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		// Place button at the right side of the menu bar
		const char* recompileText = TE_ICON_GEARS " Recompile";
		const float buttonWidth = ImGui::CalcTextSize( recompileText ).x + ImGui::GetStyle().FramePadding.x * 2.0f;
		ImGui::SetCursorPosX( ImGui::GetWindowWidth() - buttonWidth - ImGui::GetStyle().ItemSpacing.x );
		if ( ImGui::SmallButton( recompileText ) )
		{

		}

		ImGui::EndMenuBar();
	}

	void ScriptEditorPanel::DrawOpenedScripts()
	{
		const float openedScriptTabsSizeY = ImGui::GetTextLineHeightWithSpacing();

		if ( ImGui::BeginChild( "OpenedScripts", ImVec2( 0, openedScriptTabsSizeY ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
		{
			const float minTabWidth = 100.0f;
			const float tabSpacing = 10.0f;

			for ( auto& openedScript : m_OpenedScripts )
			{
				std::string tabName = openedScript.first.GetFilename().ToString();
				const bool selected = m_CurrentOpenedFile == openedScript.first;
				float tabWidth = minTabWidth;
				if ( selected )
					tabWidth = Math::Max(
						tabWidth,
						ImGui::CalcTextSize( tabName.c_str() ).x + ImGui::GetStyle().FramePadding.x * 2.0f );

				if ( ImGui::Selectable( tabName.c_str(), selected, ImGuiSelectableFlags_AllowOverlap, ImVec2( tabWidth, openedScriptTabsSizeY ) ) )
				{
					m_CurrentOpenedFile = openedScript.first;
				}

				ImGui::SameLine();
			}
			ImGui::EndChild();
		}
	}

	void ScriptEditorPanel::SetTheme( ETheme a_Theme )
	{
		const TextEditor::Palette* palette = nullptr;
		switch ( a_Theme )
		{
		case ETheme::Dark:
			palette = &TextEditor::GetDarkPalette();
			break;
		case ETheme::Light:
			palette = &TextEditor::GetLightPalette();
			break;
		case ETheme::BlueRetro:
			palette = &TextEditor::GetRetroBluePalette();
			break;
		default:
			break;
		}

		if ( palette )
		{
			m_Theme = a_Theme;
			for ( auto& openedScript : m_OpenedScripts )
			{
				openedScript.second.Editor.SetPalette( *palette );
			}
		}
	}

}
#endif // IS_EDITOR