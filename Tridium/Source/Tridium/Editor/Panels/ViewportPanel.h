#pragma once
#if IS_EDITOR
#include <Tridium/Editor/Panels/Panel.h>
#include <Tridium/Graphics/RHI/RHITexture.h>

namespace Tridium {

	class Camera;
	class SceneHeirarchyPanel;

	class ViewportPanel : public Panel
	{
	public:
		ViewportPanel( const String& a_Name );
		virtual ~ViewportPanel() = default;

		virtual void OnImGuiDraw() = 0;

	protected:
		virtual bool OnKeyPressed( KeyPressedEvent& a_Event ) { return false; };

	protected:
		RHIFramebuffer m_FBO;
		Vector2 m_ViewportSize;
	};

}

#endif