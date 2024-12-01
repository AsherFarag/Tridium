#pragma once
#if IS_EDITOR
#include "Panel.h"

namespace Tridium::Editor {

    class ScriptTextFile
    {
    public:
        ScriptTextFile() = default;
        ScriptTextFile( const  IO::FilePath& a_FilePath );

        std::string& GetContent() { return m_Content; }

        inline std::string GetFileName() { return m_Path.GetFilename().ToString(); }
        inline auto& GetPath() { return m_Path; }

        bool LoadFile( const IO::FilePath& a_FilePath );
        bool SaveFile( const IO::FilePath& a_FilePath );
        bool SaveFile() { return SaveFile( GetPath().ToString() ); }

        bool Modified = false;

    private:
        static std::string GetDirectoryPath( const std::string& a_FilePath );
        static std::string GetFileName( const std::string& a_FilePath );

        IO::FilePath m_Path;
        std::string m_Content;
    };

    class ScriptEditorPanel : public Panel
    {
    public:
        ScriptEditorPanel() : Panel("Script Editor") {}

        virtual void OnImGuiDraw() override;

        void OpenFile( const IO::FilePath& a_FilePath );

    private:
        virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

        bool DisplayFileContents( ScriptTextFile& file );

        void CloseFile( uint32_t index );
        void SaveCurrentFile();
        void SaveAllFiles();

    private:
        std::vector<ScriptTextFile> m_ScriptTextFiles;
        ScriptTextFile* m_CurrentTextFile = nullptr;
    };

}

#endif // IS_EDITOR