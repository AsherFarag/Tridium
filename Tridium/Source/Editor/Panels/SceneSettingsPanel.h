#pragma once
#include "Panel.h"
#include <Tridium/Scene/Scene.h>

namespace Tridium::Editor {

    class SceneSettingsPanel : public Panel
    {
	public:
		SceneSettingsPanel();

		void OnImGuiDraw() override;

	protected:
		SharedPtr<Scene> m_Scene;
    };

}