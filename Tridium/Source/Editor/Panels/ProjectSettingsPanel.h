#pragma once
#if IS_EDITOR

#include "Panel.h"
#include <Tridium/Project/Project.h>

namespace Tridium::Editor {

    class ProjectSettingsPanel : public Panel
    {
    public:
        ProjectSettingsPanel();
		virtual ~ProjectSettingsPanel() = default;
        virtual void OnImGuiDraw() override;

    protected:
		ProjectConfiguration m_ProjectConfig;
		bool m_Modified = false;
    };

}

#endif // IS_EDITOR