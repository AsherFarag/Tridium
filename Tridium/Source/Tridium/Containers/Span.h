#pragma once
#include <span>

namespace Tridium {
	template<typename T, size_t _Extent = std::dynamic_extent>
	using Span = std::span<T, _Extent>;
}