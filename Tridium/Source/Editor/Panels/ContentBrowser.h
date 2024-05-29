#pragma once

#ifdef IS_EDITOR

#include "Panel.h"

#include <filesystem>

namespace Tridium::Editor {

	class ContentBrowser final : public Panel
	{
	public:
		ContentBrowser() : Panel("Content Browser") {}
		virtual ~ContentBrowser() = default;

		virtual void OnImGuiDraw() override;

	private:
		std::filesystem::path m_CurrentDirectory;
	};

}

#endif // IS_EDITOR