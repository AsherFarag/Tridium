#pragma once

#include <Tridium/Core/Core.h>

#include <Tridium/Core/Window.h>
#include <Tridium/Core/LayerStack.h>
#include <Tridium/Events/ApplicationEvent.h>
#include <Tridium/ImGui/ImGuiLayer.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Project/Project.h>
#include <Tridium/Core/GameInstance.h>
#include <Tridium/Physics/PhysicsEngine.h>
#include <Tridium/Rendering/GameViewport.h>

namespace Tridium {

	// Forward Declarations
	class AssetManagerBase;

	struct FrameInfo
	{
		uint32_t FPS = 0u;
		uint32_t MinFPS = 0u;
		uint32_t MaxFPS = 0u;
	};

	class Application final
	{
	public:
		Application( const std::string& a_ProjectPath );
		virtual ~Application();

		void Run();
		static void Quit();

		// - Event Handling -
		void OnEvent( Event& e );

		// - Layer Stack -
		void PushLayer( Layer* a_Layer ) { m_LayerStack.PushLayer( a_Layer ); }
		void PushOverlay( Layer* a_Overlay ) { m_LayerStack.PushOverlay( a_Overlay ); }
		void PopLayer( Layer* a_Layer, bool a_Destroy = false ) { m_LayerStack.PopLayer( a_Layer, a_Destroy ); }
		void PopOverlay( Layer* a_Overlay, bool a_Destroy = false ) { m_LayerStack.PopOverlay( a_Overlay, a_Destroy ); }

		static Application& Get() { return *s_Instance; }
		static SharedPtr<Project> GetActiveProject() { return Get().m_Project; }
		static const IO::FilePath& GetEngineAssetsDirectory() { return Get().m_EngineAssetsDirectory; }

		Window& GetWindow() { return *m_Window; }
		uint32_t GetFPS() const { return m_PrevFrameInfo.FPS; }
		double GetFrameTime() const { return 1000.0 / m_PrevFrameInfo.FPS; }
		const FrameInfo& GetFrameInfo() const { return m_PrevFrameInfo; }

		// - Scene -
		static SharedPtr<Scene> GetScene() { return s_Instance->m_ActiveScene; }
		static void SetScene( SharedPtr<Scene> a_Scene );

	private:
		void Initialize( const std::string& a_ProjectPath );
		void InitializeProject( const IO::FilePath& a_Path );
		void InitializeAssetManager();
		bool OnWindowClosed( WindowCloseEvent& e );
		void Shutdown();
		void Update();

	private:
		bool m_Running;
		UniquePtr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		GameViewport m_GameViewport;

		UniquePtr<PhysicsEngine> m_PhysicsEngine;
		SharedPtr<AssetManagerBase> m_AssetManager;
		SharedPtr<Project> m_Project;
		SharedPtr<Scene> m_ActiveScene;
		SharedPtr<GameInstance> m_GameInstance;

		FrameInfo m_PrevFrameInfo;
		uint32_t m_MaxFPS = 144u;

		IO::FilePath m_EngineAssetsDirectory = "../Tridium/EngineAssets";

	private:
		friend class AssetManager;
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	GameInstance* CreateGameInstance();
}

