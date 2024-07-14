#include "tripch.h"
#ifdef  IS_EDITOR
#include "Editor.h"

#include <Tridium/Core/Asset.h>

// Assets
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/IO/MaterialSerializer.h>

// Panels
#include "Panels/EditorPreferencesPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHeirarchyPanel.h"
#include "Panels/ScriptEditorPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/GameViewportPanel.h"
#include "Panels/MaterialEditorPanel.h"

namespace Tridium::Editor {

    std::shared_ptr<EditorApplication> EditorApplication::s_Instance = nullptr;
    std::once_flag EditorApplication::s_InitFlag;

    EditorApplication::EditorApplication()
    {
    }

    bool EditorApplication::Init()
    {
        Application::Get().GetWindow().SetTitle("Tridium Editor");
        Application::Get().GetWindow().SetIcon( "Content/Engine/Editor/Icons/EngineIcon.png" );

        s_Instance = Ref<EditorApplication>( new EditorApplication() );
        s_Instance->m_EditorLayer = new EditorLayer();
        Application::Get().PushLayer( s_Instance->m_EditorLayer );
        return true;
    }

    bool EditorApplication::Shutdown()
    {
        return true;
    }

    namespace Util {
        bool OpenFile( const fs::path& filePath )
        {
            if ( !filePath.has_extension() )
                return false;

            std::string ext = filePath.extension().string();

            if ( ext == ".lua" )
            {
                auto scriptEditor = GetEditorLayer()->PushPanel<ScriptEditorPanel>();
                scriptEditor->OpenFile( filePath );
                scriptEditor->Focus();

                return true;
            }

            return false;
        }

        bool OpenMaterial( const fs::path& filePath )
        {
            auto panel = GetEditorLayer()->PushPanel<MaterialEditorPanel>();
            panel->Focus();
            TODO( "panel->SetMaterial(MaterialLibrary::GetMaterialHandle(filePath.string())); " );
            return true;
        }

        void SaveAll()
        {
            auto& AssetLib = AssetManager::Get().GetLibrary();
            for ( auto&& [guid, asset] : AssetLib )
            {
                asset->Save();
            }
        }
        void RecompileAllShaders()
        {
            auto& AssetLib = AssetManager::Get().GetLibrary();
            for ( auto&& [guid, asset] : AssetLib )
            {
                if ( asset->GetAssetType() != EAssetType::Shader )
                    continue;

                static_cast<Shader*>( asset.get() )->Recompile();
            }
        }
    }

}

#endif //  IS_EDITOR