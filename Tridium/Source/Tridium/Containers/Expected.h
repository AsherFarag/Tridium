#pragma once
#include <expected>

namespace Tridium {

	template<typename _Value, typename _Error>
	using Expected = std::expected<_Value, _Error>;

	template<typename _Value>
	using Unexpected = std::unexpected<_Value>;

} // namespace Tridium
