#include "tripch.h"
#include "Application.h"

#ifdef IS_EDITOR
#include <Editor/EditorLayer.h>
#endif // IS_EDITOR

// TEMP ?
#include <Tridium/Rendering/Renderer.h>
#include <Tridium/Rendering/RenderCommand.h>

// TEMP
#include "GLFW/glfw3.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Scripting/Script.h>

namespace Tridium {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		TE_CORE_ASSERT( !s_Instance, "Application already exists!" );
		s_Instance = this;
		m_Window = Window::Create();
		m_Window->SetEventCallback( TE_BIND_EVENT_FN( Application::OnEvent, std::placeholders::_1 ) );

		TODO( "Setup a proper scene initialiser!" );
		m_ActiveScene = MakeRef<Scene>();

		// Initialise ImGui
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay( m_ImGuiLayer );

#ifdef IS_EDITOR

		m_Window->SetIcon( "Content/Engine/Editor/Icons/EngineIcon.png" );

		m_EditorLayer = new Editor::EditorLayer();
		m_EditorLayer->SetActiveScene( m_ActiveScene );
		PushOverlay( m_EditorLayer );

#endif // IS_EDITOR


		// Initialise the render pipeline
		RenderCommand::Init();

		// TEMP
		std::string vertexSrc =
			R"(
						#version 410

						layout(location = 0) in vec3 aPosition;
						layout(location = 1) in vec4 aColor;
						
						out vec4 vPosition;
						out vec4 vColor;			
						
						uniform mat4 uPVM;
						
						void main()
						{	
							gl_Position = uPVM * vec4(aPosition, 1);
							vPosition =  vec4(aPosition, 1);
							vColor = aColor;
						}
					)";

		std::string fragSrc =
			R"(
						#version 410 core
						
						layout(location = 0) out vec4 aColor;

						in vec4 vPosition;
						in vec4 vColor;						
						
						uniform vec4 uColour;

						void main()
						{
							//aColor = vec4( 0.3, 0.35, 1, 1 );
							//aColor = vec4( vPosition * 0.5 + 0.5, 1 );
							//aColor = vec4( vPosition.x * 0.5 + 0.5, vPosition.y * 0.5 + 0.5, 1 - (vPosition.x * 0.5 + 0.5) - (vPosition.y * 0.5 + 0.5 ), 1 );
							//aColor = vec4(vPosition.y, vPosition.y, vPosition.y, 0.5) * 0.5 + 0.5;
							//aColor = vec4(0,1,0,1);
							aColor = (vColor * uColour);
						}
					)";

		Shader::Create( vertexSrc, fragSrc, "Default" );
	}
	
	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		m_Running = true;

		auto& go1 = m_ActiveScene->InstantiateGameObject();
		go1.AddComponent<MeshComponent>();
		go1.GetTag() = "Cube";

		auto& go2 = m_ActiveScene->InstantiateGameObject();
		go2.AddComponent<CameraComponent>();
		go2.AddComponent<CameraControllerComponent>();
		go2.GetTag() = "Scene Camera";

		while ( m_Running )
		{
			Time::Update();

			m_FPS = (uint32_t)( 1.0 / Time::DeltaTime() );


			// Update Loop ========================================================================================

			#ifndef IS_EDITOR

			m_ActiveScene->Update();

			#endif // IS_EDITOR

			// ====================================================================================================



			// Render Loop ========================================================================================
			// ====================================================================================================



			// Layers =============================================================================================

			for ( Layer* layer : m_LayerStack )
				layer->OnUpdate();

			// - ImGui -
			m_ImGuiLayer->Begin();

			for ( Layer* layer : m_LayerStack )
				layer->OnImGuiDraw();

			m_ImGuiLayer->End();
			// ---------
			
			// ====================================================================================================

			m_Window->OnUpdate();
		}
	}
	
	void Application::OnEvent( Event& e )
	{
		EventDispatcher dispatcher( e );
		dispatcher.Dispatch<WindowCloseEvent>( TE_BIND_EVENT_FN( Application::OnWindowClosed, std::placeholders::_1 ) );
	
		for ( auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			( *--it )->OnEvent( e );
			if ( e.Handled )
				break;
		}
	}
	
	void Application::PushLayer( Layer* layer )
	{
		m_LayerStack.PushLayer( layer );
	}
	
	void Application::PushOverlay( Layer* overlay )
	{
		m_LayerStack.PushOverlay( overlay );
	}
	
	bool Application::OnWindowClosed( WindowCloseEvent& e )
	{
		m_Running = false;
		return true;
	}

}
