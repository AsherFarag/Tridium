#include "tripch.h"
#if IS_EDITOR
#include "Editor.h"
#include "EditorStyle.h"

#include <Tridium/ImGui/ImGuiLayer.h>

#include <Tridium/oldAsset/AssetManager.h>
#include "Util/AssetInfo.h"

// Assets
#include <Tridium/Graphics/oldRendering/Mesh.h>
#include <Tridium/Graphics/oldRendering/Texture.h>
#include <Tridium/Graphics/oldRendering/Shader.h>
#include <Tridium/Graphics/oldRendering/Material.h>

// Panels
#include "Panels/EditorPreferencesPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHeirarchyPanel.h"
#include "Panels/ScriptEditorPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/GameViewportPanel.h"
#include "Panels/Asset/MaterialEditorPanel.h"

namespace Tridium {

	//=======================================================================================
	// Editor Events
	MulticastDelegate<void( GameObject )> Editor::Events::OnGameObjectSelected{};

	Editor::Editor( CmdLineArgs a_CmdLine )
		: Application( std::move( a_CmdLine ) )
	{
		m_EditorLayer = PushOverlay<EditorLayer>();

		// Set Window title and icon
		m_Window->SetTitle( "Tridium Editor" );
		m_Window->SetIcon( ( Engine::Get()->GetEngineAssetsDirectory() / "Editor/Icons/EngineIcon.png" ).ToString() );

		m_Style.SetTheme( EditorStyle::ETheme::Midnight );
	}

	Editor::~Editor()
	{
		s_Instance = nullptr;
	}

	void Editor::OnUpdate()
	{
	}

}

#endif //  IS_EDITOR
