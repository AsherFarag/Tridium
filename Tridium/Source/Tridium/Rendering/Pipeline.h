#pragma once

namespace Tridium {

	enum class EDepthCompareOperator
	{
		None = 0,
		Never,
		NotEqual,
		Less,
		LessOrEqual,
		Greater,
		GreaterOrEqual,
		Equal,
		Always,
	};

}