#pragma once
#ifdef IS_EDITOR
#include "EditorLayer.h"
#include "Tridium/Core/Core.h"

namespace Tridium {
	class Asset;
}

namespace Tridium::Editor {

	class EditorApplication
	{
	public:
		static SharedPtr<EditorApplication> Get() { return s_Instance; }
		static bool Init();
		static bool Shutdown();

		EditorLayer* GetEditorLayer() { return m_EditorLayer; }

	private:
		EditorLayer* m_EditorLayer;

	private:
		EditorApplication();
		EditorApplication( const EditorApplication& ) = delete;
		EditorApplication& operator=( const EditorApplication& ) = delete;

		static SharedPtr<EditorApplication> s_Instance;
		static std::once_flag s_InitFlag;
	};

	inline EditorLayer* GetEditorLayer() { return EditorApplication::Get()->GetEditorLayer(); }
	inline SharedPtr<Scene> GetActiveScene() { return EditorApplication::Get()->GetEditorLayer()->GetActiveScene();}
};

#endif // IS_EDITOR