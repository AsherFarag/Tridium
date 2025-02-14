#pragma once
#if IS_EDITOR
#include <Tridium/ImGui/ImGui.h>

namespace Tridium {

	class EditorStyle
	{
	public:
		enum class ETheme
		{
			Midnight,
			Light,
			Dark,
			RetroTech
		};

		struct Pallete
		{
			ImColor Red = { 1.0f, 0.25f, 0.35f, 1.0f };
			ImColor Green = { 0.5f, 1.0f, 0.25f, 1.0f };
			ImColor Blue = { 0.25f, 0.5f, 1.0f, 1.0f };
			ImColor Black = { 0.15f, 0.1f, 0.2f, 1.f };
			ImColor Yellow = { 1.0f, 1.0f, 0.25f, 1.0f };
			ImColor Orange = { 1.0f, 0.85f, 0.00f, 1.0f };
		} Colors;

		void SetTheme( ETheme a_Theme );
	};

}

#endif // IS_EDITOR
