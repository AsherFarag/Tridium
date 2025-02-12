#pragma once
#include "EditorConfig.h"
#include "EditorLayer.h"
#include "EditorEvents.h"
#include "EditorPayload.h"
#include "EditorStyle.h"
#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	// Forward Declarations
	namespace Editor::Internal {
		bool Init( EditorConfig a_Config );
		void Shutdown();
	}

	//=================================================================================================
	// Editor Application
	//  The global editor application class
	class EditorApplication final : public ISingleton<EditorApplication, true, true, true>
	{
	public:
		EditorPayloadManager& GetPayloadManager() { return Get()->m_PayloadManager; }
		EditorLayer* GetEditorLayer() { return m_EditorLayer; }
		EditorStyle& GetStyle() { return m_Style; }

	private:
		EditorLayer* m_EditorLayer;
		EditorPayloadManager m_PayloadManager;
		EditorStyle m_Style;

	private:
		bool Init( EditorConfig a_Config );
		bool Shutdown();

		friend bool Editor::Internal::Init( EditorConfig a_Config );
		friend void Editor::Internal::Shutdown();
	};

} // namespace Tridium