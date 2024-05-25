#pragma once
#ifdef IS_EDITOR

#include <Tridium/Core/Layer.h>

#include <Tridium/Rendering/RenderCommand.h>
#include <Tridium/Rendering/Renderer.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/framebuffer.h>

#include "EditorCamera.h"
#include "Panels/ContentBrowser.h"
#include "Panels/SceneHeirarchy.h"
#include "Panels/ScriptEditor.h"

namespace Tridium::Editor {

	class EditorLayer final : public Layer
	{
	public:
		EditorLayer( const std::string& name = "EditorLayer" );
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnImGuiDraw() override;
		virtual void OnEvent( Event& e ) override;

	private:
		// - ImGui Editor -
		void DrawMenuBar();
		void DrawEditorCameraViewPort();
		void DrawSceneToolBar();

	private:
		EditorCamera m_EditorCamera;
		Ref<Framebuffer> m_EditorCameraFBO;
		Vector2 m_ViewportSize;

		PanelStack m_PanelStack;

		ContentBrowser* m_ContentBrowser;
		SceneHeirarchy* m_SceneHeirarchy;
	};

}

#endif // IS_EDITOR