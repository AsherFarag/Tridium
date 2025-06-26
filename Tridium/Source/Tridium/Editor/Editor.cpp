#include "tripch.h"
#if IS_EDITOR
#include "Editor.h"
#include "EditorStyle.h"



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
