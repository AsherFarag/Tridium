#pragma once
#ifdef IS_EDITOR

#include <Tridium/Core/Layer.h>
#include <Tridium/Events/Eventsfwd.h>

#include "Panels/Panel.h"

namespace Tridium {
	class Scene;
	class Texture;
}

namespace Tridium::Editor {
	class EditorCamera;
	class ContentBrowserPanel;
	class SceneHeirarchyPanel;
	class EditorViewportPanel;
	class GameViewportPanel;


	enum class SceneState
	{
		None = 0,
		Edit,		// The scene does not update but the Editor Camera can move and interact with the scene
		//Simulate,	// The scene updates and the Editor Camera can move and interact with the scene
		Play		// The scene updates and runs from the scenes main camera
	};

	struct UIToolBar
	{
		Ref<Texture> PlayButtonIcon;
		Ref<Texture> PauseButtonIcon;
		Ref<Texture> StopButtonIcon;
		Ref<Texture> StepOnceButtonIcon;
		Ref<Texture> SimulateButtonIcon;

		UIToolBar();
		void OnImGuiDraw();
	};

	class EditorLayer final : public Layer
	{
	public:
		EditorLayer( const std::string& name = "EditorLayer" );
		virtual ~EditorLayer();

		// Layer Overrides
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnImGuiDraw() override;
		virtual void OnEvent( Event& e ) override;

		static EditorLayer& Get() { return *s_Instance; }

		// - Panels -
		template <typename T, typename... Args> static inline T* PushPanel( Args&&... args ) { return Get().m_PanelStack.PushPanel<T>( std::forward<Args>( args )... ); }
		template <typename T> static inline T* GetPanel() { return Get().m_PanelStack.GetPanel<T>(); }

		Ref<EditorCamera> GetEditorCamera() { return m_EditorCamera; }

		void SetActiveScene( const Ref<Scene>& a_Scene ) { m_ActiveScene = a_Scene; }
		Ref<Scene> GetActiveScene() const { return m_ActiveScene; }
		void OnBeginScene();
		void OnEndScene();

		bool LoadScene( const std::string& filepath );
		bool SaveScene( const std::string& filepath );

		static bool OpenFile( const fs::path& filePath );

	public:
		SceneState CurrentSceneState = SceneState::Edit;

	private:
		bool OnKeyPressed( KeyPressedEvent& e );

		void DrawMenuBar();

	private:
		Ref<Scene> m_ActiveScene;

		Ref<EditorCamera> m_EditorCamera;

		PanelStack m_PanelStack;
		ContentBrowserPanel* m_ContentBrowser;
		SceneHeirarchyPanel* m_SceneHeirarchy;
		EditorViewportPanel* m_EditorViewportPanel;
		GameViewportPanel* m_GameViewportPanel;
		UIToolBar m_UIToolBar;

	private:
		static EditorLayer* s_Instance;
	};

}

#endif // IS_EDITOR