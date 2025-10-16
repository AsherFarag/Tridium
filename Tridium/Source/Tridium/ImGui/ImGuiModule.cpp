#include "tripch.h"
#include "ImGuiModule.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Engine/Engine.h>
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/ImGui/Backends/ImGuiBackend_RHI.h>

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

		Application::AddOnTick( TickGroups::Render, []() { ImGuiModule::Get()->Render(); } );
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

	void ImGuiModule::Render()
	{
		GetImGuiLayer()->Begin();

		for ( const auto& layer : Application::GetLayerStack() )
			layer->OnImGuiDraw();

		GetImGuiLayer()->End();
	}

} // namespace Tridium