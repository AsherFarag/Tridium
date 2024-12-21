#pragma once
#include <Tridium/Core/Color.h>

namespace Tridium::Debug {

	class Colors
	{
	public:
		static constexpr Color Red{ 1.0f, 0.0f, 0.0f, 1.0f };
		static constexpr Color Green{ 0.0f, 1.0f, 0.0f, 1.0f };
		static constexpr Color Blue{ 0.0f, 0.0f, 1.0f, 1.0f };
		static constexpr Color Yellow{ 1.0f, 1.0f, 0.0f, 1.0f };
		static constexpr Color Cyan{ 0.0f, 1.0f, 1.0f, 1.0f };
		static constexpr Color Magenta{ 1.0f, 0.0f, 1.0f, 1.0f };
		static constexpr Color White{ 1.0f, 1.0f, 1.0f, 1.0f };
		static constexpr Color Black{ 0.0f, 0.0f, 0.0f, 1.0f };
	};

} // namespace Tridium::Debug