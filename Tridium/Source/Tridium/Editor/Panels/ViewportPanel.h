#pragma once
#if IS_EDITOR
#include "Panel.h"
#include <Tridium/Graphics/Rendering/framebuffer.h>

#include "ImGuizmo.h"


namespace Tridium {
	class Camera;
}

namespace Tridium {

	class SceneHeirarchyPanel;

	class ViewportPanel : public Panel
	{
	public:
		ViewportPanel(const std::string& name);
		virtual ~ViewportPanel() = default;

		virtual void OnImGuiDraw() = 0;

	protected:
		virtual bool OnKeyPressed( KeyPressedEvent& e ) { return false; };

	protected:
		SharedPtr<Framebuffer> m_FBO;
		Vector2 m_ViewportSize;
	};

}

#endif