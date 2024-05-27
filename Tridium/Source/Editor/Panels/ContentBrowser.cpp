#include "tripch.h"
#ifdef IS_EDITOR

#include "ContentBrowser.h"

#include "imgui.h"

namespace Tridium::Editor {

	void ContentBrowser::OnImGuiDraw()
	{
		ImGuiBegin();

		ImGuiEnd();
	}

}

#endif // IS_EDITOR