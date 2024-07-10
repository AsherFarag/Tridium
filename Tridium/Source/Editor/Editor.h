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
		static Ref<EditorApplication> Get() { return s_Instance; }
		static bool Init();
		static bool Shutdown();

		EditorLayer* GetEditorLayer() { return m_EditorLayer; }

	private:
		EditorLayer* m_EditorLayer;

	private:
		EditorApplication();
		EditorApplication( const EditorApplication& ) = delete;
		EditorApplication& operator=( const EditorApplication& ) = delete;

		static Ref<EditorApplication> s_Instance;
		static std::once_flag s_InitFlag;
	};

	inline EditorLayer* GetEditorLayer() { return EditorApplication::Get()->GetEditorLayer(); }

	inline Ref<Scene> GetActiveScene() { return EditorApplication::Get()->GetEditorLayer()->GetActiveScene();}

	namespace Util {
		bool OpenFile( const fs::path& filePath );
		bool OpenMaterial( const fs::path& filePath );
		void SaveAll();
	}
};

#endif // IS_EDITOR