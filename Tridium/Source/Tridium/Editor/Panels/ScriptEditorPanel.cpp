#include "tripch.h"
#if IS_EDITOR
#include "ScriptEditorPanel.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Editor/EditorUtil.h>
#include <Tridium/Reflection/Reflection.h>
#include <Tridium/Asset/EditorAssetManager.h>

#include <fstream>
#include <sstream>

namespace Tridium {

	ScriptEditorPanel::ScriptEditorPanel()
		: Panel( TE_ICON_CODE " Script Editor" )
	{
		SetTheme( ETheme::Dark );

		m_LuaLanguageDefinition = TextEditor::LanguageDefinition::LuaScript();
		m_LuaLanguageDefinition.mAutoIndentation = true;

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
		OpenedScript* openedScript = GetOpenedScript( m_CurrentOpenedScript );

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar;
		if ( openedScript && openedScript->IsModified )
			windowFlags |= ImGuiWindowFlags_UnsavedDocument;

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

			openedScript->IsModified |= openedScript->Editor.IsTextChanged();
		}

		// Draw opened scripts tabs
		DrawOpenedScripts();

		ImGuiEnd();
	}

	void ScriptEditorPanel::OpenHandle( const LuaScriptHandle& a_Handle )
	{
		SharedPtr<ScriptAsset> script = AssetManager::GetAsset<ScriptAsset>( a_Handle );
		if ( !script )
		{
			ASSERT( false, "Failed to open script handle" );
			return;
		}

		const auto& metaData = EditorAssetManager::Get()->GetAssetMetaData( a_Handle );
		if ( !metaData.IsValid() || metaData.AssetType != EAssetType::LuaScript )
		{
			ASSERT( false, "Failed to retrieve Asset Meta Data" );
			return;
		}

		OpenedScript openedScript;
		openedScript.Path = metaData.Path;
		openedScript.Editor.SetLanguageDefinition( m_LuaLanguageDefinition );
		openedScript.Editor.SetText( script->GetSource() );

		m_CurrentOpenedScript = a_Handle;
		m_OpenedScripts[a_Handle] = std::move( openedScript );
	}

	void ScriptEditorPanel::SaveHandle( const LuaScriptHandle& a_Handle )
	{
		OpenedScript* openedScript = GetOpenedScript( a_Handle );
		if ( !openedScript )
		{
			return;
		}

		SharedPtr<ScriptAsset> script = AssetManager::GetAsset<ScriptAsset>( a_Handle );
		if ( !script )
		{
			ASSERT( false, "Failed to save script handle" );
			return;
		}

		script->SetSource( openedScript->Editor.GetText() );
		EditorAssetManager::Get()->SaveAsset( a_Handle );

		openedScript->IsModified = false;
	}

	void ScriptEditorPanel::OpenFile( const FilePath& a_FilePath )
	{
		const auto& metaData = EditorAssetManager::Get()->GetAssetMetaData( a_FilePath );
		LuaScriptHandle handle = metaData.Handle;
		if ( !metaData.IsValid() )
		{
			handle = EditorAssetManager::Get()->ImportAsset( a_FilePath );
		}

		OpenHandle( handle );
	}

	bool ScriptEditorPanel::OnKeyPressed( const KeyPressedEvent& e )
	{
		const bool control = Input::IsKeyPressed( EInputKey::LeftControl ) || Input::IsKeyPressed( EInputKey::LeftControl );
		const bool shift = Input::IsKeyPressed( EInputKey::LeftShift ) || Input::IsKeyPressed( EInputKey::RightShift );

		switch ( e.KeyCode )
		{
		case EInputKey::S:
			if ( control )
			{
				SaveHandle( m_CurrentOpenedScript );
				return true;
			}
			break;
		case EInputKey::R:
			if ( control )
			{
				if ( shift )
				{
					RecompileAllScripts();
				}
				else
				{
					RecompileScript( m_CurrentOpenedScript );
				}
				return true;
			}
			break;
		default:
			break;
		}

		return false;
	}

	OpenedScript* ScriptEditorPanel::GetOpenedScript( const LuaScriptHandle& a_Handle )
	{
		auto it = m_OpenedScripts.find( a_Handle );
		if ( it == m_OpenedScripts.end() )
		{
			return nullptr;
		}

		return &it->second;
	}

	void ScriptEditorPanel::DrawMenuBar()
	{
		ImGui::ScopedStyleCol menuSepCol( ImGuiCol_Separator, ImVec4( 1, 1, 1, 0.75f ) );

		if ( !ImGui::BeginMenuBar() )
			return;

		if ( ImGui::BeginMenu( "File" ) )
		{
			if ( ImGui::MenuItem( "Open" ) )
			{
				Util::OpenLoadFileDialog( ( Engine::Get()->GetActiveProject().GetAssetDirectory() / "NewLuaScript.lua" ).ToString(), [this]( const FilePath& a_FilePath )
					{
						OpenFile( a_FilePath );
					} );
			}

			if ( ImGui::MenuItem( "Save", "Ctrl + S" ) )
			{
				SaveHandle( m_CurrentOpenedScript );
			}

			if ( ImGui::MenuItem( "Save As" ) )
			{
				Util::OpenSaveFileDialog( ( Engine::Get()->GetActiveProject().GetAssetDirectory() / "NewLuaScript.lua" ).ToString(), [this]( const FilePath& a_FilePath )
					{
						OpenedScript* openedScript = GetOpenedScript( m_CurrentOpenedScript );
						if ( openedScript )
						{
							std::ofstream file( a_FilePath.ToString() );
							file << openedScript->Editor.GetText();
							file.close();
						}
					} );
			}

			if ( ImGui::MenuItem( "Save All" ) )
			{
				for ( auto& openedScript : m_OpenedScripts )
				{
					SaveHandle( openedScript.first );
				}
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

		ImGui::Separator();

		if ( ImGui::BeginMenu( "Scripts" ) )
		{
			if ( ImGui::MenuItem( "Recompile", "Ctrl + R" ) )
			{
				RecompileScript( m_CurrentOpenedScript );
			}

			if ( ImGui::MenuItem( "Recompile All", "Ctrl + Shift + R" ) )
			{
				RecompileAllScripts();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	void ScriptEditorPanel::DrawOpenedScripts()
	{
		const float openedScriptTabsSizeY = ImGui::GetTextLineHeightWithSpacing();

		if ( !ImGui::BeginChild( "OpenedScripts", ImVec2( 0, openedScriptTabsSizeY ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
			return;

		const float totalWidth = ImGui::GetContentRegionAvail().x;
		const size_t tabCount = m_OpenedScripts.size();
		const float tabSpacing = 10.0f;
		const float minTabWidth = 100.0f;

		// Calculate base tab width
		const float totalSpacing = ( tabCount - 1 ) * tabSpacing;
		const float availableWidth = totalWidth - totalSpacing;
		const float baseTabWidth = Math::Max( minTabWidth, availableWidth / tabCount );

		for ( auto it = m_OpenedScripts.begin(); it != m_OpenedScripts.end(); ++it )
		{
			// Add spacing between tabs
			if ( it != m_OpenedScripts.begin() )
				ImGui::SameLine( 0, tabSpacing );

			auto& openedScript = *it;
			std::string tabName = openedScript.second.Path.GetFilename().ToString();
			const bool selected = m_CurrentOpenedScript == openedScript.first;

			// Calculate the tab width
			float tabWidth = baseTabWidth;
			if ( selected )
			{
				// Expand the selected tab by an additional amount
				const float expandedWidth = Math::Max( baseTabWidth, ImGui::CalcTextSize( tabName.c_str() ).x + ImGui::GetStyle().FramePadding.x * 2.0f );
				tabWidth = expandedWidth;
			}

			// Adjust alignment to account for the expanded tab
			ImGui::PushID( &openedScript ); // Avoid ID collisions
			if ( ImGui::Selectable( tabName.c_str(), selected, ImGuiSelectableFlags_AllowOverlap, ImVec2( tabWidth, openedScriptTabsSizeY ) ) )
			{
				m_CurrentOpenedScript = openedScript.first;
			}
			ImGui::PopID();
		}

		ImGui::EndChild();
	}

	void ScriptEditorPanel::RecompileScript( const LuaScriptHandle& a_Handle )
	{
		SharedPtr<ScriptAsset> script = AssetManager::GetAsset<ScriptAsset>( a_Handle );
		if ( !script )
		{
			ASSERT( false, "Failed to recompile script handle" );
			return;
		}

		ScriptEngine::Get()->RecompileScript( *script );
	}

	void ScriptEditorPanel::RecompileAllScripts()
	{
		for ( auto& openedScript : m_OpenedScripts )
		{
			RecompileScript( openedScript.first );
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