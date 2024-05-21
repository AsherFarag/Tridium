#pragma once

#include <Tridium/Core/Core.h>

#include <Tridium/Core/Window.h>
#include <Tridium/Core/LayerStack.h>
#include <Tridium/Events/ApplicationEvent.h>

#include <Tridium/ImGui/ImGuiLayer.h>

#include <Tridium/Scene/Scene.h>

// TEMP ?
#include <Tridium/Editor/EditorCamera.h>

// TEMP
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Buffer.h>
#include <Tridium/Rendering/VertexArray.h>

namespace Tridium
{
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
		static Scene& GetScene() { return s_Instance->m_Scene; }

	private:
		bool OnWindowClosed( WindowCloseEvent& e );

	private:
		bool m_Running;
		UniquePtr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

		Scene m_Scene;

		// TEMP ?
		EditorCamera m_EditorCamera;

		// TEMP
		Vector2 m_LastMousePosition = Vector2(0.f);
		Ref<Shader> m_Shader;
		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VBO;
		Ref<IndexBuffer> IBO;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

