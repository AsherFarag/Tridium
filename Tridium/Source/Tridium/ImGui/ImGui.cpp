#include "tripch.h"
#include "ImGui.h"
#include <Tridium/Core/Application.h>

namespace ImGui {

	float s_FontSize = 17.5f;

	ImFont* GetLightFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Application::Get().GetAssetDirectory() / "Engine" / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-Light.ttf" ).string().c_str(), s_FontSize );

		return s_Font;
	}
	ImFont* GetRegularFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Application::Get().GetAssetDirectory() / "Engine" / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-Regular.ttf" ).string().c_str(), s_FontSize );

		return s_Font;
	}
	ImFont* GetBoldFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Application::Get().GetAssetDirectory() / "Engine" / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-Bold.ttf" ).string().c_str(), s_FontSize );

		return s_Font;
	}
	ImFont* GetExtraBoldFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Application::Get().GetAssetDirectory() / "Engine" / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-ExtraBold.ttf" ).string().c_str(), s_FontSize );

		return s_Font;
	}
}