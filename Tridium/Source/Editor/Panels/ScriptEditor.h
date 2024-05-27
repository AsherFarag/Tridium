#pragma once
#ifdef IS_EDITOR
#include "Panel.h"

namespace Tridium::Editor {

    class ScriptTextFile
    {
    public:
        ScriptTextFile() = default;
        ScriptTextFile( const std::string& a_FilePath );

        std::string& GetContent() { return m_Content; }

        const std::string& GetFileName() const { return m_FileName; }
        inline std::string GetFilePath() { return m_DirectoryPath + '/' + m_FileName; }

        bool LoadFile( const std::string& a_FilePath );
        bool SaveFile( const std::string& a_FilePath );
        bool SaveFile() { return SaveFile( GetFilePath() ); }

    public:
        bool Modified = false;

    private:
        static std::string GetDirectoryPath( const std::string& a_FilePath );
        static std::string GetFileName( const std::string& a_FilePath );

    private:
        std::string m_DirectoryPath;
        std::string m_FileName;

        std::string m_Content;
    };

    class ScriptEditor : public Panel
    {
    public:
        ScriptEditor() : Panel("Script Editor") {}

        virtual void OnEvent( Event& e ) override;
        virtual void OnImGuiDraw() override;

    private:
        bool OnKeyPressed( KeyPressedEvent& e );

        bool DisplayFileContents( ScriptTextFile& file );

        void CloseFile( uint32_t index );
        void SaveCurrentFile();
        void SaveAllFiles();
        void OpenFile( const std::string& a_FilePath );

    private:
        std::vector<ScriptTextFile> m_ScriptTextFiles;
        ScriptTextFile* m_CurrentTextFile = nullptr;
    };

}

#endif // IS_EDITOR