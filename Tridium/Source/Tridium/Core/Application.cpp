#include "tripch.h"
#include "Application.h"

// TEMP ?
#include <Tridium/Rendering/Renderer.h>
#include <Tridium/Rendering/RenderCommand.h>

// TEMP
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>
#include "GLFW/glfw3.h"
#include <Tridium/Math/Math.h>

namespace Tridium {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		TE_CORE_ASSERT( !s_Instance, "Application already exists!" );
		s_Instance = this;
		m_Window = Window::Create();
		m_Window->SetEventCallback( TE_BIND_EVENT_FN( Application::OnEvent, std::placeholders::_1 ) );

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay( m_ImGuiLayer );

		#pragma region Temp Graphics Stuff

		VAO = VertexArray::Create();

		// TRI
		//float vertices[ 3 * 3 + 3 * 4 ] = {
		//	0.f, 0.5f, 0.f,		 0.2f, 0.2f, 1.0f, 1,
		//	0.5f, -0.5f, 0.f,	 0.2f, 1.0f, 0.2f, 1,
		//	-0.5f, -0.5f, 0.f,	 1.0f, 0.2f, 0.2f, 1,
		//};

		//// QUAD
		//float vertices[ 6 * 3 + 6 * 4 ] = {
		//	-1, -1,  0,		 0.4f, 0.4f, 1.0f, 1,
		//	-1,  1,  0,		 0.4f, 1.0f, 0.4f, 1,
		//	 1, -1,  0,		 1.0f, 0.4f, 0.4f, 1,
		//	 1,  1,  0,		 1.0f, 1.0f, 0.4f, 1
		//};

		float vertices[] = {
			// Position         // Color
			-1, -1,  1,         0.4f, 0.4f, 1.0f, 1,
			 1, -1,  1,         0.4f, 1.0f, 0.4f, 1,
			 1,  1,  1,         1.0f, 0.4f, 0.4f, 1,
			-1,  1,  1,         1.0f, 1.0f, 0.4f, 1,
			-1, -1, -1,         1.0f, 0.4f, 0.4f, 1,
			 1, -1, -1,         0.4f, 1.0f, 0.4f, 1,
			 1,  1, -1,         0.4f, 0.4f, 1.0f, 1,
			-1,  1, -1,         1.0f, 1.0f, 0.4f, 1
		};

		// Indices for the cube
		unsigned int indices[] = {
			// Front face
			0, 1, 2,
			2, 3, 0,
			// Back face
			4, 5, 6,
			6, 7, 4,
			// Left face
			4, 0, 3,
			3, 7, 4,
			// Right face
			1, 5, 6,
			6, 2, 1,
			// Top face
			3, 2, 6,
			6, 7, 3,
			// Bottom face
			4, 5, 1,
			1, 0, 4
		};

		VBO = VertexBuffer::Create( vertices, sizeof( vertices ));

		BufferLayout layout = {
			{ ShaderDataType::Float3, "aPosition" },
			{ ShaderDataType::Float4, "aColor" }
		};
		VBO->SetLayout( layout );
		VAO->AddVertexBuffer( VBO );

		//uint32_t indicies[ 3 ] = { 0,1,2 };
		//uint32_t indicies[ 6 ] = { 0,1,2,3,1,2 };
		IBO = IndexBuffer::Create( indices, sizeof( indices ) / sizeof(uint32_t) );
		VAO->SetIndexBuffer( IBO );

		//std::string vertexSrc =
		//R"(
		//	#version 410 core
		//	
		//	layout(location = 0) in vec3 aPosition;
		//	layout(location = 1) in vec4 aColor;
		//	out vec3 vPosition;
		//	out vec4 vColor;			

		//	void main()
		//	{
		//		vPosition = aPosition;
		//		vColor = aColor;
		//		gl_Position = vec4( aPosition, 1 );
		//	}
		//)";

		//std::string fragSrc =
		//R"(
		//	#version 410 core
		//	
		//	layout(location = 0) out vec4 aColor;
		//	in vec3 vPosition;
		//	in vec4 vColor;			
		//	
		//	uniform vec4 FColour;

		//	void main()
		//	{
		//		//vColor = vec4( 0.3, 0.35, 1, 1 );
		//		//vColor = vec4( vPosition * 0.5 + 0.5, 1 );
		//		//vColor = vec4( vPosition.x * 0.5 + 0.5, vPosition.y * 0.5 + 0.5, 1 - (vPosition.x * 0.5 + 0.5) - (vPosition.y * 0.5 + 0.5 ), 1 );
		//		//aColor = vColor;
		//		aColor = FColour;
		//	}
		//)";


		std::string vertexSrc =
			R"(
			#version 410

			layout(location = 0) in vec3 aPosition;
			
			out vec4 vPosition;
			
			uniform mat4 uPVM;
			
			void main()
			{	
			      gl_Position = uPVM * vec4(aPosition, 1);
			}
		)";

		std::string fragSrc =
			R"(
			#version 410 core
			
			layout(location = 0) out vec4 aColor;
			in vec4 vPosition;			
			
			uniform vec4 uColour;

			void main()
			{
				//vColor = vec4( 0.3, 0.35, 1, 1 );
				//vColor = vec4( vPosition * 0.5 + 0.5, 1 );
				//vColor = vec4( vPosition.x * 0.5 + 0.5, vPosition.y * 0.5 + 0.5, 1 - (vPosition.x * 0.5 + 0.5) - (vPosition.y * 0.5 + 0.5 ), 1 );
				aColor = vec4(0,1,0,1);
				aColor = uColour;
			}
		)";

		m_Shader = Shader::Create( vertexSrc, fragSrc );

#pragma endregion
	}
	
	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		m_Running = true;

		// TEMP
		auto& go = m_Scene.InstantiateGameObject();
		go.AddComponent<CameraComponent>();
		go.GetComponent<TransformComponent>().Translation = Vector3( 1, 0, 0 );
		auto& t = go.GetComponent<TransformComponent>();
		//go.AddComponent<CameraComponent>();

		TE_CORE_DEBUG( "GameObject has Transform Component: {0}", go.HasComponent<TransformComponent>() );
		TE_CORE_DEBUG( "GameObject has Tag Component: {0}", go.HasComponent<TagComponent>() );
		TE_CORE_DEBUG( "GameObject has Camera Component: {0}", go.HasComponent<CameraComponent>() );

		Matrix4 quadTransform = glm::translate( Matrix4(1), Vector3( 0 ) )
			* glm::toMat4( Quaternion( Vector3(0) ) )
			* glm::scale( Matrix4( 1 ), Vector3(1) );

		while ( m_Running )
		{
			if ( Input::IsKeyPressed( Input::KEY_ESCAPE ) )
			{
				m_Running = false;
			}

			RenderCommand::SetClearColor( { 0.1, 0.1, 0.1, 1.0 } );
			RenderCommand::Clear();

			#pragma region Input

			auto right = glm::cross( t.GetForward(), Vector3(0,1,0) );

			if ( Input::IsKeyPressed( Input::KEY_W ) )
			{
				t.Translation += t.GetForward() * 0.1f;
			}

			if ( Input::IsKeyPressed( Input::KEY_S ) )
			{
				t.Translation -= t.GetForward() * 0.1f;
			}

			if ( Input::IsKeyPressed( Input::KEY_A ) )
			{
				t.Translation -= right * 0.1f;
			}

			if ( Input::IsKeyPressed( Input::KEY_D ) )
			{
				t.Translation += right * 0.1f;
			}

			float MouseDeltaX = Input::GetMouseX() - m_LastMousePosition.x;
			float MouseDeltaY = Input::GetMouseY() - m_LastMousePosition.y;
			float DeltaTime = 1.f / 60.f;
			float m_LookSensitivity = 1.f;
			// Rotation
			if ( Input::IsMouseButtonPressed( Input::MOUSE_BUTTON_RIGHT ) )
			{
				if ( MouseDeltaX != 0.f || MouseDeltaY != 0.f )
				{
					float PitchDelta = MouseDeltaY * m_LookSensitivity * DeltaTime;
					float YawDelta = MouseDeltaX * m_LookSensitivity * DeltaTime;

					Quaternion Quat = glm::normalize( glm::cross( glm::angleAxis( -PitchDelta, right ), glm::angleAxis( -YawDelta, Vector3( 0, 1, 0 ) ) ) );
					t.Rotation = Vector3( Quat.x * Quat.w, Quat.y * Quat.w, Quat.z * Quat.w );
				}
			}

			m_LastMousePosition = Input::GetMousePosition();

#pragma endregion

			Renderer::BeginScene( go.GetComponent<CameraComponent>(), go.GetComponent<TransformComponent>() );

			m_Shader->Bind();
			Vector4 colour = Vector4( (float)( glm::sin( glfwGetTime() + 10.f ) * 0.5f + 0.5f ),
				(float)( glm::sin( glfwGetTime() ) * 0.5f + 0.5f ),
				(float)( glm::sin( glfwGetTime() - 10.f ) * 0.5f + 0.5f ), 1.0 );


			m_Shader->SetFloat4( "uColour", colour );
			Renderer::Submit( m_Shader, VAO, quadTransform );

			Renderer::EndScene();

			for ( Layer* layer : m_LayerStack )
				layer->OnUpdate();

			m_ImGuiLayer->Begin();

			for ( Layer* layer : m_LayerStack )
				layer->OnImGuiDraw();

			m_ImGuiLayer->End();

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
