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
		EditorLayer();
		virtual ~EditorLayer();

		// Layer Overrides
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnImGuiDraw() override;
		virtual void OnEvent( Event& e ) override;

		// - Panels -
		template <typename T, typename... Args>
		inline T* PushPanel( Args&&... args );
		template <typename T>
		inline T* GetPanel();

		Ref<EditorCamera> GetEditorCamera() { return m_EditorCamera; }

		void SetActiveScene( const Ref<Scene>& a_Scene ) { m_ActiveScene = a_Scene; }
		Ref<Scene> GetActiveScene() const { return m_ActiveScene; }
		void OnBeginScene();
		void OnEndScene();

		bool LoadScene( const std::string& filepath );
		bool SaveScene( const std::string& filepath );

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
	};

	template<typename T, typename ...Args>
	inline T* Editor::EditorLayer::PushPanel( Args && ...args )
	{
		return m_PanelStack.PushPanel<T>( std::forward<Args>( args )... );
	}

	template<typename T>
	inline T* Editor::EditorLayer::GetPanel()
	{
		return m_PanelStack.GetPanel<T>();
	}

}

#endif // IS_EDITOR