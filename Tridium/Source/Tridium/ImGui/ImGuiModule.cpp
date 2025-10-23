#include "tripch.h"
#include "ImGuiModule.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Engine/Engine.h>
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/ImGui/Backends/ImGuiBackend_RHI.h>
#include <Tridium/UI/UIManager.h>

namespace Tridium {

	REGISTER_ENGINE_MODULE( ImGuiModule );

	void ImGuiModule::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		// - Set up flags -
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// - Set Style -
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			style.WindowRounding = 4.f;
			style.FrameRounding = 2.f;
			style.Colors[ImGuiCol_WindowBg].w = 1.f;
		}

		// Set up Platform/Render bindings

		const EImGuiPlatform platform = EImGuiPlatform::GLFW;
		EImGuiRenderer renderer = EImGuiRenderer::Unknown;
		switch ( RHI::GetRHIType() )
		{
			case ERHInterfaceType::OpenGL:
				renderer = EImGuiRenderer::OpenGL;
				break;
			case ERHInterfaceType::Vulkan:
				renderer = EImGuiRenderer::Vulkan;
				break;
			case ERHInterfaceType::DirectX11:
				renderer = EImGuiRenderer::DX11;
				break;
			case ERHInterfaceType::DirectX12:
				renderer = EImGuiRenderer::DX12;
				break;
			default:
				ENSURE( false, "Unsupported RHI type for ImGui layer!" );
				break;
		}

		m_PlatformBackend = IPlatformImGuiInterface::Create( platform, renderer );

		ENSURE( m_PlatformBackend->Init( Application::Get()->GetWindow().GetNativeWindow(), true, renderer ),
				"Failed to initialize ImGui platform backend!" );

		ENSURE( ImGui_ImplRHI_Init( RHI::GetDynamicRHI() ),
				"Failed to initialize ImGui renderer backend!" );

		const auto cmdListDesc = RHICommandListDesc{}
			.SetEnableImmediateExecution( false )
			.SetQueueType( ERHICommandQueueType::Graphics )
			.SetName( "ImGui Layer Command List" );

		m_CmdList = RHI::CreateCommandList( cmdListDesc );

		m_ImGuiLayer = Application::PushOverlay<ImGuiLayer>();

		Application::AddOnTick( TickGroups::BeginUIDraw, []() { ImGuiModule::Get()->BeginRender(); } );
		Application::AddOnTick( TickGroups::DrawUI, []() { ImGuiModule::Get()->Render(); } );
		Application::AddOnTick( TickGroups::EndUIDraw, []() { ImGuiModule::Get()->EndRender(); } );
	}

	void ImGuiModule::Shutdown()
	{
		Application::PopOverlay( m_ImGuiLayer );
		m_ImGuiLayer = nullptr;

		m_PlatformBackend->Shutdown();
		ImGui_ImplRHI_Shutdown();

		m_PlatformBackend.reset();

		m_CmdList = nullptr;

		ImGui::DestroyContext();
	}

	void ImGuiModule::BeginRender()
	{
		ImGui_ImplRHI_NewFrame();
		ImGuiModule::GetPlatformBackend()->NewFrame();
		ImGui::NewFrame();

		static bool opt_Fullscreen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration;
		if ( opt_Fullscreen )
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos( viewport->WorkPos );
			ImGui::SetNextWindowSize( viewport->WorkSize );
			ImGui::SetNextWindowViewport( viewport->ID );

			ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

			window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
		ImGui::Begin( Engine::Get()->GetActiveProject().Config.Name.c_str(), nullptr, window_flags );
		ImGui::PopStyleVar();

		if ( opt_Fullscreen )
			ImGui::PopStyleVar( 2 );

		// Init Dock Space
		static const ImGuiID dockspace_id = ImGui::GetID( "EditorDockSpace" );
		ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), dockspace_flags );
	}

	void ImGuiModule::Render()
	{
	}

	void ImGuiModule::EndRender()
	{
		ImGui::End();

		// Size Initialization
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2( Application::Get()->GetWindow().GetWidth(), Application::Get()->GetWindow().GetHeight() );

		ImGui::Render();

		IRHISwapChain* swapChain = RHI::GetSwapChain();
		if ( !swapChain )
		{
			ENSURE( false, "Swap chain is null!" );
			return;
		}

		// Set up the command list for rendering ImGui

		ImGui_ImplRHI_RenderDrawData( ImGui::GetDrawData(), RHI::GetDynamicRHI(), swapChain->GetBackBuffer() );

		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			//m_PlatformBackend->RestoreCallbacks();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

} // namespace Tridium
