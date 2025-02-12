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

    bool Internal::Init( EditorConfig a_Config )
    {
		EditorApplication::Singleton::Construct();
        if ( !EditorApplication::Get()->Init( a_Config ) )
		{
			EditorApplication::Singleton::Destroy();
			return false;
		}

		TODO( "Load Editor Preferences" );
		// Set the editor style.
		GetStyle().SetTheme( EditorStyle::ETheme::Midnight );

		return true;
    }

	void Internal::Shutdown()
	{
		EditorApplication::Get()->Shutdown();
		EditorApplication::Singleton::Destroy();
	}

}

#endif //  IS_EDITOR
