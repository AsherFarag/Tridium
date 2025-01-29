#pragma once
#include "RHIResource.h"

namespace Tridium {

	enum class ERHITopology : uint8_t
	{
		Point = 0,
		Line,
		Triangle,
		Quad,
		COUNT,
		Unknown,
	};

	struct RHIBufferElement
	{
		const char* Name;
		ERHIDataType Type;
		uint16_t Components;
		uint16_t Offset;

		uint32_t Size() const { return RHI::GetDataTypeSize( Type ) * Components; }
	};

	struct RHIBufferLayout
	{
		uint32_t Stride;
		FixedArray<RHIBufferElement, RHIQuery::MaxVertexAttributes> Elements;

		RHIBufferLayout() = default;
		RHIBufferLayout( const std::initializer_list<RHIBufferElement>& a_Elements )
			: Elements( a_Elements )
		{
			// Calculate offsets and stride
			uint32_t offset = 0;
			Stride = 0;
			for ( auto& element : Elements )
			{
				element.Offset = offset;
				offset += RHI::GetDataTypeSize( element.Type ) * element.Components;
				Stride += element.Size();
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
		RHIBufferLayout Layout;
	};

} // namespace Tridium