#include "tripch.h"

#ifdef IS_EDITOR
#include "EditorLayer.h"
#include "imgui.h"
#include "EditorPreferences.h"

namespace Tridium::Editor {

	namespace Styles {

		static void Midnight()
		{
			ImGui::GetStyle() = ImGuiStyle();

			ImVec4* colors = ImGui::GetStyle().Colors;
			ImGuiStyle& style = ImGui::GetStyle();

			colors[ ImGuiCol_Text ] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
			colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
			colors[ ImGuiCol_WindowBg ] = ImVec4( 0.10f, 0.10f, 0.10f, 1.00f );
			colors[ ImGuiCol_ChildBg ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
			colors[ ImGuiCol_PopupBg ] = ImVec4( 0.19f, 0.19f, 0.19f, 0.92f );
			colors[ ImGuiCol_Border ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.29f );
			colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.24f );
			colors[ ImGuiCol_FrameBg ] = ImVec4( 0.05f, 0.05f, 0.05f, 0.54f );
			colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.19f, 0.19f, 0.19f, 0.54f );
			colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.20f, 0.22f, 0.23f, 1.00f );
			colors[ ImGuiCol_TitleBg ] = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );
			colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.06f, 0.06f, 0.06f, 1.00f );
			colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );
			colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
			colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.05f, 0.05f, 0.05f, 0.54f );
			colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.34f, 0.34f, 0.34f, 0.54f );
			colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.40f, 0.40f, 0.40f, 0.54f );
			colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.56f, 0.56f, 0.56f, 0.54f );
			colors[ ImGuiCol_CheckMark ] = ImVec4( 0.33f, 0.67f, 0.86f, 1.00f );
			colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.34f, 0.34f, 0.34f, 0.54f );
			colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.56f, 0.56f, 0.56f, 0.54f );
			colors[ ImGuiCol_Button ] = ImVec4( 0.05f, 0.05f, 0.05f, 0.54f );
			colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.19f, 0.19f, 0.19f, 0.54f );
			colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.20f, 0.22f, 0.23f, 1.00f );
			colors[ ImGuiCol_Header ] = ImVec4( 0.70f, 0.72f, 0.83f, 0.52f );
			colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.80f, 0.80f, 0.90f, 0.36f );
			colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.90f, 0.90f, 1.00f, 0.65f );
			colors[ ImGuiCol_Separator ] = ImVec4( 0.28f, 0.28f, 0.28f, 0.29f );
			colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.44f, 0.44f, 0.44f, 0.29f );
			colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.40f, 0.44f, 0.47f, 1.00f );
			colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0.28f, 0.28f, 0.28f, 0.29f );
			colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.44f, 0.44f, 0.44f, 0.29f );
			colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.40f, 0.44f, 0.47f, 1.00f );
			colors[ ImGuiCol_Tab ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.52f );
			colors[ ImGuiCol_TabHovered ] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
			colors[ ImGuiCol_TabActive ] = ImVec4( 0.40f, 0.40f, 0.40f, 0.36f );
			colors[ ImGuiCol_TabUnfocused ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.52f );
			colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
			colors[ ImGuiCol_DockingPreview ] = ImVec4( 0.33f, 0.67f, 0.86f, 1.00f );
			colors[ ImGuiCol_DockingEmptyBg ] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ ImGuiCol_PlotLines ] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ ImGuiCol_PlotHistogram ] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ ImGuiCol_TableHeaderBg ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.52f );
			colors[ ImGuiCol_TableBorderStrong ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.52f );
			colors[ ImGuiCol_TableBorderLight ] = ImVec4( 0.28f, 0.28f, 0.28f, 0.29f );
			colors[ ImGuiCol_TableRowBg ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
			colors[ ImGuiCol_TableRowBgAlt ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.06f );
			colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.20f, 0.22f, 0.23f, 1.00f );
			colors[ ImGuiCol_DragDropTarget ] = ImVec4( 0.33f, 0.67f, 0.86f, 1.00f );
			colors[ ImGuiCol_NavHighlight ] = ImVec4( 1.00f, 0.00f, 0.00f, 1.00f );
			colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.00f, 0.00f, 0.00f, 0.70f );
			colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 1.00f, 0.00f, 0.00f, 0.20f );
			colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 1.00f, 0.00f, 0.00f, 0.35f );

			style.WindowPadding = ImVec2( 8.00f, 8.00f );
			style.FramePadding = ImVec2( 5.00f, 2.00f );
			style.CellPadding = ImVec2( 6.00f, 6.00f );
			style.ItemSpacing = ImVec2( 6.00f, 6.00f );
			style.ItemInnerSpacing = ImVec2( 6.00f, 6.00f );
			style.TouchExtraPadding = ImVec2( 0.00f, 0.00f );
			style.IndentSpacing = 25;
			style.ScrollbarSize = 15;
			style.GrabMinSize = 10;
			style.WindowBorderSize = 1;
			style.ChildBorderSize = 0;
			style.PopupBorderSize = 1;
			style.FrameBorderSize = 1;
			style.TabBorderSize = 1;
			style.WindowRounding = 0;
			style.ChildRounding = 4;
			style.FrameRounding = 3;
			style.PopupRounding = 4;
			style.ScrollbarRounding = 9;
			style.GrabRounding = 3;
			style.LogSliderDeadzone = 4;
			style.TabRounding = 4;
		}

	}

	EditorLayer::EditorLayer( const std::string& name )
	{

	}

	void EditorLayer::OnAttach()
	{
		Styles::Midnight();

		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		m_EditorCameraFBO = Framebuffer::Create( FBOspecification );
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnUpdate()
	{
		m_EditorCamera.OnUpdate();

		m_EditorCameraFBO->Bind();
		{
			Application::GetScene()->Render( m_EditorCamera, m_EditorCamera.GetViewMatrix() );
		}
		m_EditorCameraFBO->Unbind();
	}

	void EditorLayer::OnImGuiDraw()
	{
		static bool dockspace_Open = true;
		static bool opt_Fullscreen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if ( opt_Fullscreen )
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos( viewport->WorkPos );
			ImGui::SetNextWindowSize( viewport->WorkSize );
			ImGui::SetNextWindowViewport( viewport->ID );

			ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
		ImGui::Begin( "DockSpace Window", &dockspace_Open, window_flags );
		ImGui::PopStyleVar();
		{
			if ( opt_Fullscreen )
				ImGui::PopStyleVar( 2 );

			// Init Dock Space
			ImGuiID dockspace_id = ImGui::GetID( "EditorDockSpace" );
			ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), dockspace_flags );

			DrawMenuBar();
			DrawEditorCameraViewPort();
			m_ContentBrowser.OnImGuiDraw();
			m_SceneHeirarchy.OnImGuiDraw();
		}
		ImGui::End();
	}

	void EditorLayer::OnEvent( Event& e )
	{
	}

	void EditorLayer::DrawMenuBar()
	{
		if ( !ImGui::BeginMenuBar() )
			return;

		if ( ImGui::BeginMenu( "File" ) )
		{
			// Project
			if ( ImGui::MenuItem( "New Project" ) )
				TE_CORE_INFO( "New Project" );

			if ( ImGui::MenuItem( "Open Project" ) )
				TE_CORE_INFO( "Open Project" );

			if ( ImGui::MenuItem( "Save Project" ) )
				TE_CORE_INFO( "Save Project" );

			ImGui::Separator();

			// Scene
			if ( ImGui::MenuItem( "New Scene" ) )
				TE_CORE_INFO( "New Scene" );

			if ( ImGui::MenuItem( "Open Scene" ) )
				TE_CORE_INFO( "Open Scene" );

			if ( ImGui::MenuItem( "Save Scene" ) )
				TE_CORE_INFO( "Save Scene" );

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if ( ImGui::BeginMenu( "Edit" ) )
		{
			if ( ImGui::BeginMenu( "Editor Style" ) )
			{
				if ( ImGui::MenuItem( "Classic" ) ) ImGui::StyleColorsClassic();
				if ( ImGui::MenuItem( "Light" ) ) ImGui::StyleColorsLight();
				if ( ImGui::MenuItem( "Dark" ) ) ImGui::StyleColorsDark();
				if ( ImGui::MenuItem( "Midnight" ) ) Styles::Midnight();

				ImGui::EndMenu();
			}

			if ( ImGui::MenuItem( "Editor Preferences" ) )
				Application::Get().PushOverlay( new EditorPreferences() );

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "View" ) )
		{
			if ( ImGui::MenuItem( "Recompile", "Ctrl+R" ) )
				TE_CORE_INFO( "Not Implemented!" );

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Script" ) )
		{
			if ( ImGui::MenuItem( "Recompile", "Ctrl+R" ) )
				TE_CORE_INFO( "Not Implemented!" );

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
		
	}

	void EditorLayer::DrawEditorCameraViewPort()
	{
		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(2.f, 2.f) );
		ImGui::Begin( "Viewport ##" );
		{
			m_EditorCamera.Focused = ImGui::IsWindowFocused();
			Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

			uint64_t textureID = m_EditorCameraFBO->GetColorAttachmentID();
			ImGui::Image( reinterpret_cast<void*>( textureID ), ImGui::GetContentRegionAvail(), ImVec2{ 0, 1 }, ImVec2{ 1, 0 } );

			if ( m_ViewportSize != regionAvail )
			{
				m_ViewportSize = regionAvail;
				m_EditorCamera.SetViewportSize( regionAvail.x, regionAvail.y );
				m_EditorCameraFBO->Resize( regionAvail.x, regionAvail.y );
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::DrawSceneToolBar()
	{
	}
}

#endif // IS_EDITOR