#pragma once
#include <Tridium/ImGui/ImGui.h>
#ifdef IS_EDITOR

namespace Tridium::Editor {

	class EditorStyle
	{
	public:
		static constexpr ImColor Red = {1.0f, 0.25f, 0.35f, 1.0f };
		static constexpr ImColor Green = { 0.5f, 1.0f, 0.25f, 1.0f };
		static constexpr ImColor Blue = { 0.25f, 0.5f, 1.0f, 1.0f };
		static constexpr ImColor Black = { 0.15f, 0.1f, 0.2f, 1.f };
		static constexpr ImColor Yellow = { 1.0f, 1.0f, 0.25f, 1.0f };
		static constexpr ImColor Orange = { 1.0f, 0.5f, 0.25f, 1.0f };
	};

}

#endif // IS_EDITOR