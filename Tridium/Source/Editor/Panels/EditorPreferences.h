#pragma once
#ifdef IS_EDITOR
#include "Panel.h"

namespace Tridium::Editor {

    class EditorPreferences : public Panel
    {
	public:
		virtual ~EditorPreferences() = default;

		virtual void OnImGuiDraw() override;
    };

}

#endif // IS_EDITOR