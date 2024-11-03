#pragma once

namespace Tridium {

	enum class ERenderMode
	{
		None = 0,
		Forward,
		Deferred,
	};

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

	enum class ECullMode
	{
		None = 0,
		Front,
		Back,
	};

}