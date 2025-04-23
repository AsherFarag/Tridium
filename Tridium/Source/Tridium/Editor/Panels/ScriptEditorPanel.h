#pragma once
#if IS_EDITOR
#include "Panel.h"
#include "ImTextEdit/TextEditor.h"
#include <Tridium/Asset/AssetType.h>

namespace Tridium {

	struct OpenedScript
	{
		FilePath Path;
		TextEditor Editor;
		bool IsModified = false;
	};

    class ScriptEditorPanel : public Panel
    {
		enum class ETheme : uint8_t
		{
			Dark,
			Light,
			BlueRetro,
		};

    public:
        ScriptEditorPanel();

        virtual void OnImGuiDraw() override;

		void OpenHandle( const LuaScriptHandle& a_Handle );
		void SaveHandle( const LuaScriptHandle& a_Handle );
        void OpenFile( const FilePath& a_FilePath );

    private:
        virtual bool OnKeyPressed( const KeyPressedEvent& e ) override;
		OpenedScript* GetOpenedScript( const LuaScriptHandle& a_Handle );
		void SetTheme( ETheme a_Theme );

		void DrawMenuBar();
		void DrawOpenedScripts();

		void RecompileScript( const LuaScriptHandle& a_Handle );
		void RecompileAllScripts();

	private:
		std::unordered_map<LuaScriptHandle, OpenedScript> m_OpenedScripts;
		LuaScriptHandle m_CurrentOpenedScript;
		ETheme m_Theme = ETheme::Dark;
		TextEditor::LanguageDefinition m_LuaLanguageDefinition;
    };

}

#endif // IS_EDITOR