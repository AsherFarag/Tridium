#pragma once

#ifdef IS_EDITOR

namespace Tridium::Editor {

	class Panel
	{
	public:
		Panel() = default;
		virtual ~Panel() = default;

		virtual void OnImGuiDraw() {}
	};

}

#endif // IS_EDITOR