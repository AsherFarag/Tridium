#pragma once
#if IS_EDITOR
#include "EditorApplication.h"

namespace Tridium {

	//=================================================================================================
	// Editor Namespace
	//  A namespace for editor specific functions and utilities
	namespace Editor {

		inline EditorPayloadManager& GetPayloadManager() { return EditorApplication::Get()->GetPayloadManager(); }
		inline EditorLayer* GetEditorLayer() { return EditorApplication::Get() ? EditorApplication::Get()->GetEditorLayer() : nullptr; }

		// Get the editor style
		inline EditorStyle& GetStyle() { return EditorApplication::Get()->GetStyle(); }

		// Get the color palette of the current editor style
		inline EditorStyle::Pallete& GetPallete() { return GetStyle().Colors; }
	}
	//=================================================================================================

	namespace Editor::Internal {
		bool Init( EditorConfig a_Config );
		void Shutdown();
	}
};

#endif // IS_EDITOR