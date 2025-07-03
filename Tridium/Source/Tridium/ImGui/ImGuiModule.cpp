#include "tripch.h"
#include "ImGuiModule.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Engine/Engine.h>

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
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// - Set Style -
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			style.WindowRounding = 4.f;
			style.FrameRounding = 2.f;
			style.Colors[ ImGuiCol_WindowBg ].w = 1.f;
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
		m_RendererBackend = IRendererImGuiInterface::Create( renderer );
		if ( !m_PlatformBackend || !m_RendererBackend )
		{
			ENSURE( false, "Failed to create ImGui backend interfaces!" );
		}

		ENSURE( m_PlatformBackend->Init( Application::Get()->GetWindow().GetNativeWindow(), true, renderer ),
				"Failed to initialize ImGui platform backend!" );

		ENSURE( m_RendererBackend->Init( RHI::GetDynamicRHI() ),
				"Failed to initialize ImGui renderer backend!" );

		const auto cmdListDesc = RHICommandListDesc{}
			.SetEnableImmediateExecution( false )
			.SetQueueType( ERHICommandQueueType::Graphics )
			.SetName( "ImGui Layer Command List" );
		m_CmdList = RHI::CreateCommandList( cmdListDesc );

		m_ImGuiLayer = Application::PushOverlay<ImGuiLayer>();
	}

	void ImGuiModule::Shutdown()
	{
		Application::PopOverlay( m_ImGuiLayer );
		m_ImGuiLayer = nullptr;

		m_PlatformBackend->Shutdown(); 
		m_RendererBackend->Shutdown();

		m_PlatformBackend.reset();
		m_RendererBackend.reset();

		m_CmdList = nullptr;

		ImGui::DestroyContext();
	}
}
