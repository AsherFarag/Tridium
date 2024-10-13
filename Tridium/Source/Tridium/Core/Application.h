#pragma once

#include <Tridium/Core/Core.h>

#include <Tridium/Core/Window.h>
#include <Tridium/Core/LayerStack.h>
#include <Tridium/Events/ApplicationEvent.h>
#include <Tridium/ImGui/ImGuiLayer.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Project/Project.h>
#include <Tridium/Core/GameInstance.h>

namespace Tridium
{
#ifdef IS_EDITOR
	namespace Editor { class EditorLayer; }
#endif // IS_EDITOR

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		static void Quit();

		void OnEvent( Event& e );

		void PushLayer( Layer* a_Layer ) { m_LayerStack.PushLayer( a_Layer ); }
		void PushOverlay( Layer* a_Overlay ) { m_LayerStack.PushOverlay( a_Overlay ); }
		void PopLayer( Layer* a_Layer, bool a_Destroy = false ) { m_LayerStack.PopLayer( a_Layer, a_Destroy ); }
		void PopOverlay( Layer* a_Overlay, bool a_Destroy = false ) { m_LayerStack.PopOverlay( a_Overlay, a_Destroy ); }

		static Application& Get() { return *s_Instance; }
		static SharedPtr<Project> GetActiveProject() { return Get().m_Project; }
		static const IO::FilePath& GetAssetDirectory() { return Get().m_Project->GetAssetDirectory(); }

		Window& GetWindow() { return *m_Window; }
		uint32_t GetFPS() const { return m_FPS; }

		// - Scene -
		static SharedPtr<Scene> GetScene() { return s_Instance->m_ActiveScene; }
		static void SetScene( SharedPtr<Scene> a_Scene ) { s_Instance->m_ActiveScene = a_Scene; }
	private:
		bool OnWindowClosed( WindowCloseEvent& e );

		void Shutdown();

	private:
		bool m_Running;
		UniquePtr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		SharedPtr<GameInstance> m_GameInstance;

		SharedPtr<Project> m_Project;
		SharedPtr<Scene> m_ActiveScene;

		uint32_t m_FPS = 0u;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	GameInstance* CreateGameInstance();
}

