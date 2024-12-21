#pragma once
#if IS_EDITOR
#include "Panel.h"

namespace Tridium::Editor {

    class EditorPreferencesPanel : public Panel
    {
	public:
		EditorPreferencesPanel() : Panel( "Editor Preferences" ) {}
		virtual ~EditorPreferencesPanel() = default;

		virtual void OnImGuiDraw() override;
    };

}

#endif // IS_EDITOR