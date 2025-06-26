#pragma once

namespace Tridium {

	enum class ERenderMode : uint8_t
	{
		None = 0,
		Forward,
		Deferred,
	};

	enum class EDepthCompareOperator : uint8_t
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

	enum class ECullMode : uint8_t
	{
		None = 0,
		Front,
		Back,
	};

	enum class EBlendMode : uint8_t
	{
		None = 0,
		Alpha,
		Additive,
	};

	enum class EFaces : uint8_t
	{
		Front = 0,
		Back,
		FrontAndBack,
	};

	enum class EPolygonMode : uint8_t
	{
		Fill = 0,
		Line,
		Point,
	};
}