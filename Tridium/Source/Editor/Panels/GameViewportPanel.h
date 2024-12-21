#pragma once
#if IS_EDITOR

#include <Tridium/ECS/GameObject.h>
#include "ViewportPanel.h"

namespace Tridium {
	class Camera;
}

namespace Tridium::Editor {

	class GameViewportPanel : public ViewportPanel
	{
	public:
		GameViewportPanel( GameObject camera = GameObject() );
		virtual ~GameViewportPanel() = default;

		virtual void OnImGuiDraw() override;

	private:
		std::optional< std::tuple<Camera&, Matrix4, Vector3> > GetSceneCamera() const;

	private:
		GameObject m_Camera;
	};

}

#endif // IS_EDITOR