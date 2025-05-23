#pragma once
#if IS_EDITOR

#include <Tridium/Core/Layer.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Events/Eventsfwd.h>

#include "Panels/Panel.h"

// Forward Declarations
namespace Tridium {
	class Scene;

	namespace Editor {
		class EditorCamera;
		class ContentBrowserPanel;
		class SceneHeirarchyPanel;
		class EditorViewportPanel;
		class GameViewportPanel;
	}
}

namespace Tridium::Editor {

	enum class SceneState
	{
		None = 0,
		Edit,		// The scene does not update but the Editor Camera can move and interact with the scene
		//Simulate,	// The scene updates and the Editor Camera can move and interact with the scene
		Play		// The scene updates and runs from the scenes main camera
	};

	struct UIToolBar
	{
		SharedPtr<Texture> PlayButtonIcon;
		SharedPtr<Texture> PauseButtonIcon;
		SharedPtr<Texture> StopButtonIcon;
		SharedPtr<Texture> StepOnceButtonIcon;
		SharedPtr<Texture> SimulateButtonIcon;

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
		template <typename T>
		inline T* GetOrEmplacePanel();

		SharedPtr<EditorCamera> GetEditorCamera() { return m_EditorCamera; }

		void SetActiveScene( const SharedPtr<Scene>& a_Scene );
		SharedPtr<Scene> GetActiveScene() const;
		void OnBeginScene();
		void OnEndScene();

		SceneState CurrentSceneState = SceneState::Edit;

	private:
		bool OnKeyPressed( KeyPressedEvent& e );
		void DrawMenuBar();

	private:
		// Used as a temporary storage for the current scene when in a PIE session.
		// Once a PIE session is ended, the scene is restored to the active scene.
		UniquePtr<Scene> m_SceneSnapshot;

		SharedPtr<EditorCamera> m_EditorCamera;

		PanelStack m_PanelStack;
		ContentBrowserPanel* m_ContentBrowser;
		SceneHeirarchyPanel* m_SceneHeirarchy;
		EditorViewportPanel* m_EditorViewportPanel;
		GameViewportPanel* m_GameViewportPanel;
		UIToolBar m_UIToolBar;

		friend ::Tridium::Application;
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

	template<typename T>
	inline T* EditorLayer::GetOrEmplacePanel()
	{
		return m_PanelStack.GetOrEmplacePanel<T>();
	}

}

#endif // IS_EDITOR