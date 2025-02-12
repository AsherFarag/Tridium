#pragma once
#ifdef IS_EDITOR

#include "Panel.h"

namespace Tridium {

	class StatsPanel : public Panel
	{
	public:
		StatsPanel();
		virtual ~StatsPanel() = default;

		virtual void OnImGuiDraw() override;
	};

}

#endif // IS_EDITOR