#pragma once
#include "RHIResource.h"

namespace Tridium {

	enum class ERHITopology : uint8_t
	{
		Point = 0,
		Line,
		Triangle,
		LineStrip,
		TriangleStrip,
		COUNT,
		Unknown,
	};

	struct RHIVertexAttribute
	{
		StringView Name;            // Name of the attribute
		RHIVertexElementType Type;  // Data type of the attribute
		uint16_t Offset;            // Offset in bytes from the start of the vertex layout
	};

	struct RHIVertexLayout
	{
		// Stride of the vertex layout in bytes
		uint32_t Stride;
		// Array of vertex attributes
		InlineArray<RHIVertexAttribute, RHIQuery::MaxVertexAttributes> Elements;

		constexpr RHIVertexLayout() = default;
		constexpr RHIVertexLayout( const std::initializer_list<RHIVertexAttribute>& a_Elements )
			: Elements( a_Elements )
		{
			// Calculate offsets and stride
			Stride = 0;
			for ( auto& element : Elements )
			{
				element.Offset = Stride;
				Stride += element.Type.GetSize();
			}
		}
	};

	RHI_RESOURCE_BASE_TYPE( IndexBuffer )
	{
		Span<const Byte> InitialData = {};
		ERHIUsageHint UsageHint = ERHIUsageHint::Default;
		ERHIDataType DataType = ERHIDataType::UInt16;
	};

	RHI_RESOURCE_BASE_TYPE( VertexBuffer )
	{
		Span<const Byte> InitialData = {};
		ERHIUsageHint UsageHint = ERHIUsageHint::Default;
		RHIVertexLayout Layout;
	};

} // namespace Tridium