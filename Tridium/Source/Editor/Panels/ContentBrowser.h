#pragma once

#ifdef IS_EDITOR

#include "Panel.h"

namespace Tridium::Editor {

	class ContentBrowser final : public Panel
	{
	public:
		ContentBrowser() : Panel("Content Browser") {}
		virtual ~ContentBrowser() = default;

		virtual void OnImGuiDraw() override;
	};

}

#endif // IS_EDITOR