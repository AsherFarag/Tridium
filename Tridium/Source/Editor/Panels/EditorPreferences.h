#pragma once
#ifdef IS_EDITOR
#include "Panel.h"

namespace Tridium::Editor {

    class EditorPreferences : public Panel
    {
	public:
		EditorPreferences() : Panel( "Editor Preferences" ) {}
		virtual ~EditorPreferences() = default;

		virtual void OnImGuiDraw() override;
    };

}

#endif // IS_EDITOR