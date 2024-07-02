#pragma once
#ifdef IS_EDITOR

#include <Tridium/ECS/GameObject.h>
#include "ViewportPanel.h"

namespace Tridium::Editor {

	class GameViewportPanel : public ViewportPanel
	{
	public:
		GameViewportPanel( GameObject camera = GameObject() );
		virtual ~GameViewportPanel() = default;

		virtual void OnImGuiDraw() override;

	public:
		GameObject m_Camera;
	};

}

#endif // IS_EDITOR