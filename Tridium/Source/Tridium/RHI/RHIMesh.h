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

	struct RHIBufferElement
	{
		StringView Name;     // Name of the element
		RHIDataType Type;    // Data type of the element
		uint16_t Offset;     // Offset in bytes from the start of the buffer

		constexpr RHIBufferElement( StringView a_Name, RHIDataType::EComponentType a_Type, uint8_t a_Components = 1u )
			: Name( a_Name ), Type( { a_Type, a_Components } ), Offset( 0 )
		{
		}

		constexpr RHIBufferElement( StringView a_Name, RHIDataType a_Type )
			: Name( a_Name ), Type( a_Type ), Offset( 0 )
		{
		}
	};

	struct RHIBufferLayout
	{
		uint32_t Stride;
		InlineArray<RHIBufferElement, RHIQuery::MaxVertexAttributes> Elements;

		RHIBufferLayout() = default;
		RHIBufferLayout( const std::initializer_list<RHIBufferElement>& a_Elements )
			: Elements( a_Elements )
		{
			// Calculate offsets and stride
			Stride = 0;
			for ( auto& element : Elements )
			{
				element.Offset = Stride;
				Stride += element.Type.Size();
			}
		}
	};

	RHI_RESOURCE_BASE_TYPE( IndexBuffer )
	{
		Span<const Byte> InitialData = {};
		ERHIUsageHint UsageHint = ERHIUsageHint::Default;
		RHIDataType DataType = RHIDataType::UInt16;
	};

	RHI_RESOURCE_BASE_TYPE( VertexBuffer )
	{
		Span<const Byte> InitialData = {};
		ERHIUsageHint UsageHint = ERHIUsageHint::Default;
		RHIBufferLayout Layout;
	};

} // namespace Tridium