#pragma once
#ifdef IS_EDITOR
#include "Panel.h"
#include <Tridium/Rendering/framebuffer.h>

#include "ImGuizmo.h"


namespace Tridium {
	class Camera;
}

namespace Tridium::Editor {
	class SceneHeirarchy;

	enum class EGizmoState
	{
		Translate = ImGuizmo::TRANSLATE,
		Rotate = ImGuizmo::ROTATE,
		Scale = ImGuizmo::SCALE,
		Universal_Scale = ImGuizmo::SCALEU
	};

	class ViewportPanel : public Panel
	{
	public:
		ViewportPanel();
		virtual ~ViewportPanel() = default;

		virtual void OnImGuiDraw() override;

	public:
		Camera* ViewedCamera;
		Matrix4 CameraViewMatrix = Matrix4(1.0f); // To be set by the manager of this viewport

	private:
		virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

	private:
		EGizmoState m_GizmoState = EGizmoState::Translate;
		SceneHeirarchy* m_SceneHeirarchy = nullptr;
		Ref<Framebuffer> m_ViewedCameraFBO;
		Vector2 m_ViewportSize;
	};

}

#endif