#include "tripch.h"
#if IS_EDITOR
#include "Editor.h"
#include "EditorStyle.h"

#include <Tridium/Asset/AssetManager.h>
#include "Util/AssetInfo.h"

// Assets
#include <Tridium/Graphics/Rendering/Mesh.h>
#include <Tridium/Graphics/Rendering/Texture.h>
#include <Tridium/Graphics/Rendering/Shader.h>
#include <Tridium/Graphics/Rendering/Material.h>

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
        Application::Get()->GetWindow().SetTitle("Tridium Editor");
        Application::Get()->GetWindow().SetIcon(  Engine::Get()->GetEngineAssetsDirectory() / "Editor/Icons/EngineIcon.png" );

		AssetTypeManager::s_Instance = MakeUnique<AssetTypeManager>();
		AssetTypeManager::s_Instance->Initialize();

		s_Instance = SharedPtr<EditorApplication>( new EditorApplication() );
        s_Instance->m_EditorLayer = new EditorLayer();
        Application::Get()->PushLayer( s_Instance->m_EditorLayer );

		Style::SetTheme( Style::ETheme::Midnight );

        return true;
    }

    bool EditorApplication::Shutdown()
    {
        return true;
    }

}

#endif //  IS_EDITOR