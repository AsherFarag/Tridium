#pragma once

#include <Tridium/Core/Core.h>

#include <Tridium/Core/Window.h>
#include <Tridium/Core/LayerStack.h>
#include <Tridium/Events/ApplicationEvent.h>
#include <Tridium/ImGui/ImGuiLayer.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Project/Project.h>

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

		void PushLayer( Layer* layer );
		void PushOverlay( Layer* overlay );

		static Application& Get() { return *s_Instance; }
		static const fs::path& GetAssetDirectory() { return Get().m_Project.GetAssetDirectory(); }

		Window& GetWindow() { return *m_Window; }
		uint32_t GetFPS() const { return m_FPS; }

		// - Scene -
		static Ref<Scene> GetScene() { return s_Instance->m_ActiveScene; }

	private:
		bool OnWindowClosed( WindowCloseEvent& e );

		void Shutdown();

	private:
		bool m_Running;
		UniquePtr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

		Project m_Project;
		Ref<Scene> m_ActiveScene;

		uint32_t m_FPS = 0u;

	#ifdef IS_EDITOR
		
		Editor::EditorLayer* m_EditorLayer;

	#endif // IS_EDITOR


	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

