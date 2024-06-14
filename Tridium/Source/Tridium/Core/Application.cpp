#include "tripch.h"
#include "Application.h"

#ifdef IS_EDITOR
#include <Editor/EditorLayer.h>
#endif // IS_EDITOR

// TEMP ?
#include <Tridium/Rendering/Renderer.h>
#include <Tridium/Rendering/RenderCommand.h>

// TEMP
using namespace entt::literals;
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

		//m_Project = new Project("Sandbox", "Content", "Content");
		ProjectSerializer::Deserialize( m_Project, "Content/Sandbox.TEproject" );

#ifdef IS_EDITOR

		m_Window->SetIcon( "Content/Engine/Editor/Icons/EngineIcon.png" );

		m_EditorLayer = new Editor::EditorLayer();
		m_EditorLayer->SetActiveScene( m_ActiveScene );
		PushOverlay( m_EditorLayer );

#endif // IS_EDITOR


		// Initialise the render pipeline
		RenderCommand::Init();

		// TEMP
		// entt meta demo
		entt::meta<TransformComponent>()
			.data<&TransformComponent::Position>( "Position"_hs )
			.data<&TransformComponent::Rotation>( "Rotation"_hs )
			.data<&TransformComponent::Scale>( "Scale"_hs );
	}
	
	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		m_Running = true;

		// TEMP
		// entt meta demo
		auto data = entt::resolve<TransformComponent>().data( "Position"_hs );
		TE_CORE_DEBUG( data.type().info().name() );

		while ( m_Running )
		{
			Time::Update();

			m_FPS = (uint32_t)( 1.0 / Time::DeltaTime() );

			// Update Loop ========================================================================================

			#ifndef IS_EDITOR

			m_ActiveScene->OnUpdate();

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

	void Application::Quit()
	{
	#ifdef IS_EDITOR
		Get().m_EditorLayer->OnEndScene();
	#else
		Get().Shutdown();
	#endif // IS_EDITOR
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
		Shutdown();
		return true;
	}

	void Application::Shutdown()
	{
		m_Running = false;
	}

}
