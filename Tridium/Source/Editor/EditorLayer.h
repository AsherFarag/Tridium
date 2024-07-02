#pragma once
#ifdef IS_EDITOR

#include <Tridium/Core/Layer.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Events/Eventsfwd.h>

#include "Panels/Panel.h"

#include <Tridium/Rendering/RenderCommand.h>
#include <Tridium/Rendering/Renderer.h>
#include <Tridium/Rendering/Shader.h>

#include "EditorCamera.h"

#include <Tridium/Rendering/Texture.h>

namespace Tridium::Editor {

	class ContentBrowser;
	class SceneHeirarchy;
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
		Ref<Texture2D> PlayButtonIcon;
		Ref<Texture2D> PauseButtonIcon;
		Ref<Texture2D> StopButtonIcon;
		Ref<Texture2D> StepOnceButtonIcon;
		Ref<Texture2D> SimulateButtonIcon;

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

		template <typename T, typename... Args>
		inline T* PushPanel( Args&&... args ) { return m_PanelStack.PushPanel<T>( std::forward<Args>( args )... ); }

		template <typename T>
		inline T* GetPanel() { return m_PanelStack.GetPanel<T>(); }

		static EditorLayer& Get() { return *s_Instance; }
		EditorCamera& GetEditorCamera() { return m_EditorCamera; }

		void SetActiveScene( const Ref<Scene>& a_Scene ) { m_ActiveScene = a_Scene; }
		Ref<Scene> GetActiveScene() const { return m_ActiveScene; }
		void OnBeginScene();
		void OnEndScene();

		bool LoadScene( const std::string& filepath );
		bool SaveScene( const std::string& filepath );

	public:
		SceneState CurrentSceneState = SceneState::Edit;

	private:
		bool OnKeyPressed( KeyPressedEvent& e );

		void DrawMenuBar();

	private:
		Ref<Scene> m_ActiveScene;

		EditorCamera m_EditorCamera;

		PanelStack m_PanelStack;
		ContentBrowser* m_ContentBrowser;
		SceneHeirarchy* m_SceneHeirarchy;
		EditorViewportPanel* m_EditorViewportPanel;
		GameViewportPanel* m_GameViewportPanel;
		UIToolBar m_UIToolBar;

	private:
		static EditorLayer* s_Instance;
	};

}

#endif // IS_EDITOR