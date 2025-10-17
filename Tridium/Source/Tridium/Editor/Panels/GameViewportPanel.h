#pragma once
#if IS_EDITOR

#include <Tridium/ECS/GameObject.h>
#include "ViewportPanel.h"

namespace Tridium {
	class Camera;
}

namespace Tridium {

	class GameViewportPanel : public ViewportPanel
	{
	public:
		GameViewportPanel( OldGameObject camera = OldGameObject() );
		virtual ~GameViewportPanel() = default;

		virtual void OnImGuiDraw() override;

	private:
		std::optional< std::tuple<Camera&, Matrix4, Vector3> > GetSceneCamera() const;

	private:
		OldGameObject m_Camera;
	};

}

#endif // IS_EDITOR