#include "tripch.h"
#include "Application.h"
#include "Engine/Engine.h"
#include <Tridium/IO/FileManager.h>
#include <Tridium/Debug/Profiler/ProfileSessionSerializer.h>

#if IS_EDITOR
	#include <Editor/Editor.h>
	#include <Tridium/Asset/EditorAssetManager.h>
#else
	#include <Tridium/Asset/RuntimeAssetManager.h>
#endif // IS_EDITOR

// TEMP ?
#include <Tridium/Rendering/GameViewport.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Rendering/RenderCommand.h>
#include "Tridium/IO/ProjectSerializer.h"
#include <Tridium/Scripting/ScriptEngine.h>
#include <Tridium/RHI/RHI.h>


namespace Tridium {

	Application* Application::s_Instance = nullptr;

	///////////////////////////////////////////////////////////////////////////////////////////
	Application::Application( const std::string& a_ProjectPath )
	{
		TODO( "Temp fix" );
		if ( a_ProjectPath.empty() )
		{
			m_EngineAssetsDirectory = "EngineAssets";
		}

		Initialize( a_ProjectPath );
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
	}
	
	void Application::Run()
	{
		m_Running = true;

		m_GameInstance.reset( CreateGameInstance() );
		m_GameInstance->Init();

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

		// TEMP!
		RHIConfig config;
		config.RHIType = ERHInterfaceType::DirectX12;
		bool success = RHI::Initialise( config );
		TE_CORE_INFO( "RHI Initialised: {0}", success );
		TE_CORE_INFO( "RHI Type: {0}", RHI::GetRHIName() );

		RHITextureDescriptor desc;
		desc.Dimensions[0] = 4;
		desc.Dimensions[1] = 4;
		Byte exampleImgData[4 * 4 * 4];
		for ( int i = 0; i < 4 * 4 * 4; i++ )
		{
			exampleImgData[i] = 0xFF;
		}
		desc.InitialData = exampleImgData;
		RHITextureRef tex = RHI::CreateTexture( "",);

		while ( m_Running )
		{
			PROFILE_FRAME();
			// Update Time
			const double lastFrameTime = Time::Now();
			Time::Update();
			Time::s_DeltaTime = Time::Now() - lastFrameTime;

			// Update FPS
			++frameCounter;
			fpsInterval += Time::DeltaTime();
			uint32_t curFrameRate = 1.0 / Time::DeltaTime();
			minFPS = MIN( minFPS, curFrameRate );
			maxFPS = MAX( maxFPS, curFrameRate );
			if ( fpsInterval >= 1.0 )
			{
				m_PrevFrameInfo.FPS = frameCounter;
				m_PrevFrameInfo.MinFPS = minFPS;
				m_PrevFrameInfo.MaxFPS = maxFPS;
				frameCounter = 0;
				fpsInterval -= 1.0;
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

		for ( Layer* layer : m_LayerStack )
			layer->OnUpdate();

		// - ImGui -
		m_ImGuiLayer->Begin();

		for ( int i = 0; i < m_LayerStack.NumLayers(); i++ )
			m_LayerStack[i]->OnImGuiDraw();

		m_ImGuiLayer->End();
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
	void Application::OnEvent( Event& e )
	{
		EventDispatcher dispatcher( e );
		dispatcher.Dispatch<WindowCloseEvent>( TE_BIND_EVENT_FN( Application::OnWindowClosed, 1 ) );
	
		for ( auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			( *--it )->OnEvent( e );
			if ( e.Handled )
				break;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::SetScene( SharedPtr<Scene> a_Scene )
	{
		s_Instance->m_ActiveScene = a_Scene;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Initialize( const std::string& a_ProjectPath )
	{
		// Set the singleton instance
		TE_CORE_ASSERT( !s_Instance, "Application already exists!" );
		s_Instance = this;

		// Initialise Window
		{
			m_Window = Window::Create();
			m_Window->SetEventCallback( TE_BIND_EVENT_FN( Application::OnEvent, 1 ) );
		}

		RenderCommand::Init();
		InitializeProject( a_ProjectPath );

		Engine::Construct();
		Engine::Get()->Initialize();

		Script::ScriptEngine::s_Instance = MakeUnique<Script::ScriptEngine>();
		Script::ScriptEngine::s_Instance->Init();

		InitializeAssetManager();

		// Initialise Start Scene
		{
			TE_CORE_INFO( "Loading start scene" );
			if ( SharedPtr<Scene> scene = AssetManager::GetAsset<Scene>( m_Project->GetConfiguration().StartScene ) )
			{
				m_ActiveScene = scene;
			}
			else
			{
				TE_CORE_WARN( "Failed to load start scene! - Creating new scene" );
				m_ActiveScene = MakeShared<Scene>();
			}
		}

		// Add the ImGui layer
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay( m_ImGuiLayer );

		// Initialise the editor
	#if IS_EDITOR
		Editor::EditorApplication::Init();
	#endif // IS_EDITOR
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::InitializeProject( const IO::FilePath& a_Path )
	{
		m_Project = MakeShared<Project>();

		// If a project path was provided, load the project
		if ( a_Path.Exists() )
		{
			ProjectSerializer s( m_Project );
			s.DeserializeText( a_Path );
			m_Project->GetConfiguration().ProjectDirectory = IO::FilePath( a_Path ).GetParentPath();
		}
		// Otherwise, attempt to find a project file in the current directory
		else
		{
			TE_CORE_TRACE( "No project path provided - searching for project file in '{0}'", IO::FileManager::GetWorkingDirectory().ToString() );
			IO::FilePath projectPath = IO::FileManager::GetWorkingDirectory() / "Project.tproject"; //IO::FileManager::FindFileWithExtension( ".tproject" );
			if ( projectPath.Exists() )
			{
				TE_CORE_TRACE( "Project file found at '{0}'", projectPath.ToString() );
				ProjectSerializer s( m_Project );
				s.DeserializeText( projectPath );
				m_Project->GetConfiguration().ProjectDirectory = projectPath.GetParentPath();
			}
			else
			{
				TE_CORE_WARN( "No project file found!" );
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::InitializeAssetManager()
	{
	#if IS_EDITOR
		//m_AssetManager = MakeShared<Editor::EditorAssetManager>();
	#else
		//m_AssetManager = MakeShared<RuntimeAssetManager>();
	#endif // IS_EDITOR

		m_AssetManager = MakeShared<Editor::EditorAssetManager>();
		m_AssetManager->Init();
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
		m_GameInstance->Shutdown();
		m_AssetManager->Shutdown();

	#if IS_EDITOR
		Editor::EditorApplication::Shutdown();
	#endif // IS_EDITOR

		Engine::Get()->Shutdown();
	}

} // namespace Tridium