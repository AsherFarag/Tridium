#pragma once

#include <Tridium/Core/Core.h>

#include <Tridium/Core/Window.h>
#include <Tridium/Core/LayerStack.h>
#include <Tridium/Events/ApplicationEvent.h>
#include <Tridium/ImGui/ImGuiLayer.h>
#include <Tridium/Scene/Scene.h>

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

		void OnEvent( Event& e );

		void PushLayer( Layer* layer );
		void PushOverlay( Layer* overlay );

		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *m_Window; }

		// - Scene -
		static Ref<Scene> GetScene() { return s_Instance->m_ActiveScene; }

		uint32_t GetFPS() const { return m_FPS; }

	private:
		bool OnWindowClosed( WindowCloseEvent& e );

	private:
		bool m_Running;
		UniquePtr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

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

