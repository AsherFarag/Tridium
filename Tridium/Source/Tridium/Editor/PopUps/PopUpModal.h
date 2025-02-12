#pragma once
#if IS_EDITOR
#include <string>
#include <Tridium/ImGui/ImGui.h>

struct PopUpModal
{
	std::string Name;
	ImGuiWindowFlags Flags;
	bool Open = true;

	PopUpModal( const std::string& name, ImGuiWindowFlags flags )
		: Name( name ), Flags( flags ) {}

	void OnImGuiDraw()
	{
		if ( ImGui::BeginPopupModal( Name.c_str(), nullptr, Flags ) )
		{

			ImGui::EndPopup();
		}
	}
};

#endif // IS_EDITOR