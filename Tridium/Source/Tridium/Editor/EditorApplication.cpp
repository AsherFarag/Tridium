#include "tripch.h"
#include "EditorApplication.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Engine/Engine.h>

namespace Tridium {

	bool EditorApplication::Init( EditorConfig a_Config )
    {
        Application::Get()->GetWindow().SetTitle( "Tridium Editor" );
        Application::Get()->GetWindow().SetIcon( ( Engine::Get()->GetEngineAssetsDirectory() / "Editor/Icons/EngineIcon.png" ).ToString() );

        m_EditorLayer = new EditorLayer();
        Application::Get()->PushLayer( m_EditorLayer );

        return true;
    }

    bool EditorApplication::Shutdown()
    {
        return true;
    }


}