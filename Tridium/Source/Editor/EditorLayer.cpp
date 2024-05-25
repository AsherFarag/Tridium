#include "tripch.h"

#ifdef IS_EDITOR
#include "EditorLayer.h"
#include "imgui.h"
#include "Panels/EditorPreferences.h"

namespace Tridium::Editor {

	EditorLayer::EditorLayer( const std::string& name )
	{

	}

	void EditorLayer::OnAttach()
	{
		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		m_EditorCameraFBO = Framebuffer::Create( FBOspecification );

		m_SceneHeirarchy = new SceneHeirarchy();
		m_ContentBrowser = new ContentBrowser();
		m_PanelStack.PushPanel( m_SceneHeirarchy );
		m_PanelStack.PushPanel( m_ContentBrowser );
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

			for ( int i = m_PanelStack.size() - 1; i >= 0; --i )
			{
				m_PanelStack[ i ]->OnImGuiDraw();
			}

			ImGui::Begin( "Stats" );
			ImGui::Text( "FPS: %i", Application::Get().GetFPS() );
			ImGui::End();
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
			if ( ImGui::MenuItem( "Editor Preferences" ) )
				m_PanelStack.PushPanel( new EditorPreferences() );

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "View" ) )
		{

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Script" ) )
		{
			if ( ImGui::MenuItem( "Open Script Editor" ) )
				m_PanelStack.PushPanel( new ScriptEditor() );

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