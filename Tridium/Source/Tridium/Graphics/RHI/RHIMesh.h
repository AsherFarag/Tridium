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
		// Name of the attribute
		StringView Name;
		// Data type of the attribute
		ERHIVertexElementType Type;
		// Offset in bytes from the start of the vertex layout.
		// Calculated by the RHIVertexLayout.
		uint16_t Offset;

		constexpr RHIVertexAttribute() : Name(), Type( ERHIVertexElementType::None ), Offset( 0 ) {}
		constexpr RHIVertexAttribute( StringView a_Name, ERHIVertexElementType a_Type )
			: Name( a_Name ), Type( a_Type ), Offset( 0 ) {}
		constexpr RHIVertexAttribute( const char* a_Name, ERHIVertexElementType a_Type )
			: Name( a_Name ), Type( a_Type ), Offset( 0 ) {}
	};

	struct RHIVertexLayout
	{
		// Stride of the vertex layout in bytes
		uint32_t Stride = 0;
		// Array of vertex attributes
		InlineArray<RHIVertexAttribute, RHIQuery::MaxVertexAttributes, /*_ForceConstexpr*/ true> Elements;

		constexpr RHIVertexLayout() = default;
		constexpr RHIVertexLayout( const std::initializer_list<RHIVertexAttribute>& a_Elements )
			: Elements( a_Elements )
		{
			// Calculate offsets and stride
			Stride = 0;
			for ( auto& element : Elements )
			{
				element.Offset = Stride;
				Stride += GetRHIVertexElementTypeSize( element.Type );
			}
		}
	};

	DEFINE_RHI_RESOURCE( IndexBuffer )
	{
		Span<const Byte> InitialData = {};
		ERHIUsageHint UsageHint = ERHIUsageHint::Default;
		ERHIDataType DataType = ERHIDataType::UInt16;
	};

	DEFINE_RHI_RESOURCE( VertexBuffer )
	{
		Span<const Byte> InitialData = {};
		ERHIUsageHint UsageHint = ERHIUsageHint::Default;
		RHIVertexLayout Layout;
	};

} // namespace Tridium