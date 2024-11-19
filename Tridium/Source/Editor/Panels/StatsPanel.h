#pragma once
#include "Panel.h"

namespace Tridium::Editor {

	class StatsPanel : public Panel
	{
	public:
		StatsPanel();
		virtual ~StatsPanel() = default;

		virtual void OnImGuiDraw() override;
	};

}