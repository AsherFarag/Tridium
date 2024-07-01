#pragma once
#ifdef IS_EDITOR
#include "Panel.h"

namespace Tridium::Editor {

    class ScriptTextFile
    {
    public:
        ScriptTextFile() = default;
        ScriptTextFile( const  fs::path& a_FilePath );

        std::string& GetContent() { return m_Content; }

        inline std::string GetFileName() { return m_Path.filename().string(); }
        inline auto& GetPath() { return m_Path; }

        bool LoadFile( const fs::path& a_FilePath );
        bool SaveFile( const fs::path& a_FilePath );
        bool SaveFile() { return SaveFile( GetPath().string() ); }

    public:
        bool Modified = false;

    private:
        static std::string GetDirectoryPath( const std::string& a_FilePath );
        static std::string GetFileName( const std::string& a_FilePath );

    private:
        fs::path m_Path;
        std::string m_Content;
    };

    class ScriptEditor : public Panel
    {
    public:
        ScriptEditor() : Panel("Script Editor") {}

        virtual void OnImGuiDraw() override;

        void OpenFile( const fs::path& a_FilePath );

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