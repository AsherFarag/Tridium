#include "tripch.h"
#include "Application.h"
#include <Tridium/Engine/Engine.h>
#include <Tridium/IO/FileManager.h>
#include <Tridium/Debug/Profiler/ProfileSessionSerializer.h>

// TEMP!
#if IS_EDITOR
	#include <Tridium/Editor/Editor.h>
#endif // IS_EDITOR

#include <Tridium/ImGui/ImGuiLayer.h>

// TEMP ?
#include <Tridium/Graphics/Rendering/GameViewport.h>
#include <Tridium/Graphics/Rendering/RenderCommand.h>
#include <Tridium/Graphics/RHI/RHI.h>

import Tridium.ECS;

namespace Tridium {

	Application* Application::s_Instance = nullptr;

	///////////////////////////////////////////////////////////////////////////////////////////

	Application::Application( CmdLineArgs a_ProjectPath )
	{
		s_Instance = this;
		m_CommandLineArgs = std::move( a_ProjectPath );
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		s_Instance = nullptr;
	}
	
	void Application::Run()
	{
		Init();

		m_Running = true;

		m_GameViewport.Init( m_Window->GetWidth(), m_Window->GetHeight() );

		uint32_t frameCounter = 0;
		double fpsInterval = 0.0;
		uint32_t minFPS = 0xFFFFFFFF;
		uint32_t maxFPS = 0;

	#if !IS_EDITOR
		if ( m_ActiveScene )
		{
			m_ActiveScene->OnBeginPlay();
		}
	#endif // IS_EDITOR

		while ( m_Running )
		{
			PROFILE_FRAME();
			// Update Time
			const double lastFrameTime = Time::Now();
			Time::Update(); // 	s_Time = glfwGetTime();
			Time::s_DeltaTime = Time::Now() - lastFrameTime;

			// Update FPS
			++frameCounter;
			fpsInterval += Time::DeltaTime();
			uint32_t curFrameRate = 1.0 / Time::DeltaTime();
			minFPS = Math::Min( minFPS, curFrameRate );
			maxFPS = Math::Max( maxFPS, curFrameRate );
			if ( fpsInterval >= 1.0 )
			{
				m_PrevFrameInfo.FPS = frameCounter;
				m_PrevFrameInfo.MinFPS = minFPS;
				m_PrevFrameInfo.MaxFPS = maxFPS;
				frameCounter = 0;
				fpsInterval = 0.0;
				minFPS = 0xFFFFFFFF;
				maxFPS = 0;
			}

			// Update Loop
			if ( !m_Window->IsMinimized() )
			{
				Update();
			}

			m_Window->OnUpdate();
		}

	#if !IS_EDITOR
		if ( m_ActiveScene )
		{
			m_ActiveScene->OnEndPlay();
		}
	#endif // IS_EDITOR

		Shutdown();
	}

	bool Application::Init()
	{
		// Initialise the Window
		m_Window = Window::Create();
		m_Window->SetEventCallback( std::bind( &Application::OnEvent, this, std::placeholders::_1 ) );

		RHIConfig config;
		config.RHIType = ERHInterfaceType::OpenGL;
		config.UseDebug = true;
		LOG( LogCategory::Rendering, Info, "'{0}' - RHI: Initialised = {1}", RHI::GetRHIName( config.RHIType ), RHI::Initialise( config ) );

		// TEMP!
		{

			//uint8_t testImgData[64 * 64 * 4];
			//for ( size_t y = 0; y < 64; y++ )
			//{
			//	for ( size_t x = 0; x < 64; x++ )
			//	{
			//		testImgData[( y * 64 + x ) * 4 + 0] = x * 4;
			//		testImgData[( y * 64 + x ) * 4 + 1] = y * 4;
			//		testImgData[( y * 64 + x ) * 4 + 2] = 255 - ( x * 2 + y * 2 );
			//		testImgData[( y * 64 + x ) * 4 + 3] = 255;
			//	}
			//}

			//RHITextureDescriptor desc;
			//desc.InitialData = testImgData;
			//desc.Dimensions[0] = 64;
			//desc.Dimensions[1] = 64;
			//desc.Format = ERHITextureFormat::RGBA8;
			//desc.Name = "My beautiful texture";

			//RHITextureRef tex = RHI::CreateTexture( desc );
			////LOG_INFO( "Successfully wrote to texture = {0}", tex->Write(desc.InitialData) );

			//// Create vertex buffer

			//RHIVertexLayout layout =
			//{
			//	{ "Position", RHIVertexElementTypes::Float3 },
			//};

			//struct Vertex
			//{
			//	Vector3 Position;
			//};

			//Vertex vertices[] =
			//{
			//	{ { 0.0, 0.0, 0.0 } },
			//	{ { 1.0, 0.0, 0.0 } },
			//	{ { 1.0, 1.0, 0.0 } },
			//	{ { 0.0, 1.0, 0.0 } },
			//};

			//RHIVertexBufferDescriptor vbDesc;
			//vbDesc.Layout = layout;
			//vbDesc.InitialData = Span<const Byte>( (const Byte*)( &vertices[0] ), sizeof(vertices));
			//vbDesc.Name = "My beautiful vertex buffer";

			//RHIVertexBufferRef vb = RHI::CreateVertexBuffer( vbDesc );

			//while ( true )
			//{
			//	m_Window->OnUpdate();
			//}
		}

		// Initialise the Engine
		EngineConfig engineConfig;
		Engine::Singleton::Construct();
		if ( !Engine::Get()->Init( std::move( engineConfig ) ) )
		{
			return false;
		}

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Update()
	{
		PROFILE_FUNCTION( ProfilerCategory::Application );

		// Update Loop ========================================================================================

	#if !IS_EDITOR

		Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Disabled );

		m_ActiveScene->OnUpdate();

	#endif // IS_EDITOR

		// ====================================================================================================



		// Render Loop ========================================================================================

	#if !IS_EDITOR

		if ( !m_ActiveScene->GetMainCamera() )
		{
			auto view = m_ActiveScene->GetECS().View<CameraComponent>();
			if ( !view.empty() )
				m_ActiveScene->SetMainCamera( view.begin()[0] );
		}

		if ( m_ActiveScene->GetMainCamera() )
		{
			// Render the scene with the main camera
			Vector3 cameraPos = m_ActiveScene->GetMainCamera()->GetGameObject().GetTransform().Position;
			m_ActiveScene->GetSceneRenderer().Render(
				m_GameViewport.GetFramebuffer(),
				m_ActiveScene->GetMainCamera()->SceneCamera,
				m_ActiveScene->GetMainCamera()->GetView(),
				cameraPos );
		}
		else
		{
			// If no camera is found, render the scene with a default camera
			m_ActiveScene->GetSceneRenderer().Render(
				m_GameViewport.GetFramebuffer(),
				Camera(),
				Matrix4( 1.0f ),
				Vector3( 0.0 ) );
		}

		m_GameViewport.Resize( m_Window->GetWidth(), m_Window->GetHeight() );
		m_GameViewport.RenderToWindow();

	#endif // !IS_EDITOR


		// ====================================================================================================



		// Layers =============================================================================================

		for ( const auto& layer : m_LayerStack )
			layer->OnUpdate();

		// - ImGui -
		ImGuiLayer* imGuiLayer = Engine::Get()->m_ImGuiLayer;
		imGuiLayer->Begin();

		for ( int i = 0; i < m_LayerStack.NumLayers(); i++ )
			m_LayerStack[i]->OnImGuiDraw();

		imGuiLayer->End();
		// ---------

		// ====================================================================================================
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Quit()
	{
	#if IS_EDITOR
		Editor::GetEditorLayer()->OnEndScene();
	#else
		Get().m_Running = false;
	#endif // IS_EDITOR
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::OnEvent( Event& a_Event )
	{
		EventDispatcher dispatcher( a_Event );
		dispatcher.Dispatch<WindowCloseEvent>( TE_BIND_EVENT_FN( Application::OnWindowClosed, 1 ) );
	
		for ( auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			if ( a_Event.Handled )
				break;

			( *--it )->OnEvent( a_Event );
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	bool Application::OnWindowClosed( WindowCloseEvent& e )
	{
		m_Running = false;
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Shutdown()
	{
		Engine::Get()->Shutdown();
		Engine::Singleton::Destroy();
	}

} // namespace Tridium