#include "tripch.h"
#ifdef  IS_EDITOR
#include "Editor.h"
#include "EditorStyle.h"

#include <Tridium/Asset/AssetManager.h>

// Assets
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Material.h>

// Panels
#include "Panels/EditorPreferencesPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHeirarchyPanel.h"
#include "Panels/ScriptEditorPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/GameViewportPanel.h"
#include "Panels/Asset/MaterialEditorPanel.h"

namespace Tridium::Editor {

    std::shared_ptr<EditorApplication> EditorApplication::s_Instance = nullptr;
    std::once_flag EditorApplication::s_InitFlag;

    EditorApplication::EditorApplication()
    {
    }

    bool EditorApplication::Init()
    {
        Application::Get().GetWindow().SetTitle("Tridium Editor");
        Application::Get().GetWindow().SetIcon(  Application::GetEngineAssetsDirectory() / "Editor/Icons/EngineIcon.png" );

        s_Instance = SharedPtr<EditorApplication>( new EditorApplication() );
        s_Instance->m_EditorLayer = new EditorLayer();
        Application::Get().PushLayer( s_Instance->m_EditorLayer );

		Style::SetTheme( Style::ETheme::Midnight );

        return true;
    }

    bool EditorApplication::Shutdown()
    {
        return true;
    }

    namespace Util {
        bool OpenFile( const IO::FilePath& filePath )
        {
            if ( !filePath.HasExtension() )
                return false;

            std::string ext = filePath.GetExtension().ToString();

            if ( ext == ".lua" )
            {
                auto scriptEditor = GetEditorLayer()->PushPanel<ScriptEditorPanel>();
                scriptEditor->OpenFile( filePath );
                scriptEditor->Focus();

                return true;
            }

            return false;
        }

        bool OpenMaterial( const IO::FilePath& filePath )
        {
            auto panel = GetEditorLayer()->PushPanel<MaterialEditorPanel>();
            panel->Focus();
            TODO( "panel->SetMaterial(MaterialLibrary::GetMaterialHandle(filePath.string())); " );
            return true;
        }

        void SaveAll()
        {
            //auto& AssetLib = AssetManager::Get()->GetLibrary();
            //for ( auto&& [guid, asset] : AssetLib )
            //{
            //    //asset->Save();
            //}
        }
        void RecompileAllShaders()
        {
            //auto& AssetLib = AssetManager::Get()->GetLibrary();
            //for ( auto&& [guid, asset] : AssetLib )
            //{
            //    if ( AssetRef<Shader> shader = asset.As<Shader>() )
            //    {
            //        shader->Recompile();
            //    }
            //}
        }
    }

}

#endif //  IS_EDITOR