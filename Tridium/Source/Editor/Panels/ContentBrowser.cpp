#include "tripch.h"
#ifdef IS_EDITOR

#include "ContentBrowser.h"

#include "imgui.h"

namespace Tridium::Editor {

	void ContentBrowser::OnImGuiDraw()
	{
		if ( !ImGui::Begin( "Content Browser" ) )
			return;

		ImGui::End();
	}

}

#endif // IS_EDITOR