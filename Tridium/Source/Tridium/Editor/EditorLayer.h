#pragma once
#if IS_EDITOR

#include <Tridium/Core/Layer.h>
#include <Tridium/Graphics/oldRendering/Texture.h>
#include <Tridium/Events/Event.h>

#include "Panels/Panel.h"

namespace Tridium {

	// Forward Declarations
	class OldScene;
	class EditorCamera;
	class ContentBrowserPanel;
	class SceneHeirarchyPanel;
	class EditorViewportPanel;
	class GameViewportPanel;

	struct UIToolBar
	{
		SharedPtr<TextureOld> PlayButtonIcon;
		SharedPtr<TextureOld> PauseButtonIcon;
		SharedPtr<TextureOld> StopButtonIcon;
		SharedPtr<TextureOld> StepOnceButtonIcon;
		SharedPtr<TextureOld> SimulateButtonIcon;

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

		void OnBeginScene();
		void OnEndScene();

		EEditorSceneState CurrentSceneState = EEditorSceneState::Edit;

	private:
		bool OnKeyPressed( const KeyPressedEvent& e );
		void DrawMenuBar();

	private:
		// Used as a temporary storage for the current scene when in a PIE session.
		// Once a PIE session is ended, the scene is restored to the active scene.
		SharedPtr<OldScene> m_SceneSnapshot;

		SharedPtr<EditorCamera> m_EditorCamera;

		PanelStack m_PanelStack;
		ContentBrowserPanel* m_ContentBrowser;
		SceneHeirarchyPanel* m_SceneHeirarchy;
		EditorViewportPanel* m_EditorViewportPanel;
		GameViewportPanel* m_GameViewportPanel;
		UIToolBar m_UIToolBar;

		friend class Application;
		friend class Editor;
	};

	template<typename T, typename ...Args>
	inline T* EditorLayer::PushPanel( Args && ...args )
	{
		return m_PanelStack.PushPanel<T>( std::forward<Args>( args )... );
	}

	template<typename T>
	inline T* EditorLayer::GetPanel()
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