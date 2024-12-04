#pragma once
#if IS_EDITOR
#include "Panel.h"
#include "ImTextEdit/TextEditor.h"

namespace Tridium::Editor {

	struct OpenedScript
	{
		IO::FilePath Path;
		TextEditor Editor;
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

        void OpenFile( const IO::FilePath& a_FilePath );

    private:
        virtual bool OnKeyPressed( KeyPressedEvent& e ) override;
		OpenedScript* GetOpenedScript( const IO::FilePath& a_FilePath );
		void SetTheme( ETheme a_Theme );

		void DrawMenuBar();
		void DrawOpenedScripts();

	private:
		std::unordered_map<IO::FilePath, OpenedScript> m_OpenedScripts;
		IO::FilePath m_CurrentOpenedFile;
		ETheme m_Theme = ETheme::Dark;
		TextEditor::LanguageDefinition m_LuaLanguageDefinition;
    };

}

#endif // IS_EDITOR