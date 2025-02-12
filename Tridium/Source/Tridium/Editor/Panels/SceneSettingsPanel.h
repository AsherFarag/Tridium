#pragma once
#if IS_EDITOR

#include "Panel.h"
#include <Tridium/Scene/Scene.h>

namespace Tridium {

    class SceneRendererPanel : public Panel
    {
	public:
		SceneRendererPanel();

		void OnImGuiDraw() override;
    };

}

#endif // IS_EDITOR