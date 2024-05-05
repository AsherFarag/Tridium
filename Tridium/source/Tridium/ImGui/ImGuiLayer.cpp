#include "tripch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include <Platform/OpenGL/ImGuiOpenGLRenderer.h>
#include "GLFW/glfw3.h"

#include <Tridium/Application.h>

namespace Tridium {

	ImGuiLayer::ImGuiLayer()
		: Layer( "ImGuiLayer" )
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		ImGui_ImplOpenGL3_Init( "#version 410" );
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnUpdate()
	{
		// Size Initialization
		ImGuiIO& io = ImGui::GetIO();
		auto& app = Application::Get();
		io.DisplaySize = ImVec2( app.GetWindow().GetWidth(), app.GetWindow().GetHeight() );

		// Timing
		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0 ? ( time - m_Time ) : ( 1.0f / 60.0f );
		m_Time = time;

		// New Frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow( &show );

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
	}

	void ImGuiLayer::OnEvent( Event& event )
	{
	}

}