#pragma once
#include "RHIResource.h"
#include <Tridium/Reflection/FieldReflection.h>

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
		ERHIFormat Type;
		// Offset in bytes from the start of the vertex layout.
		// Calculated by the RHIVertexLayout.
		uint16_t Offset;

		constexpr RHIVertexAttribute() : Name(), Type( ERHIFormat::Unknown ), Offset( 0 ) {}
		constexpr RHIVertexAttribute( StringView a_Name, ERHIFormat a_Type, uint16_t a_Offset = 0 )
			: Name( a_Name ), Type( a_Type ), Offset( a_Offset ) {}
	};

	struct RHIVertexLayout
	{
		// Stride of the vertex layout in bytes
		uint32_t Stride = 0;
		// Array of vertex attributes
		InlineArray<RHIVertexAttribute, RHIConstants::MaxVertexAttributes, /*_ForceConstexpr*/ true> Elements{};

		constexpr RHIVertexLayout() = default;
		constexpr RHIVertexLayout( const std::initializer_list<RHIVertexAttribute>& a_Elements )
			: Elements( a_Elements )
		{
			// Calculate offsets and stride
			Stride = 0;
			for ( auto& element : Elements )
			{
				element.Offset = Stride;
				Stride += GetRHIFormatInfo( element.Type ).Bytes();
			}
		}

		// Constructs a vertex layout from an aggregate type.
		// All fields of the aggregate type must be of a supported RHIFormat/RHITensor type.
		template<Concepts::Aggregate T>
		static consteval RHIVertexLayout From()
		{
			RHIVertexLayout layout{};

			ForEachField( T{},
				[&]( StringView a_FieldName, auto&& a_Field )
				{
					using FieldType = std::decay_t<decltype(a_Field)>;
					static_assert( IsRHIFormat<FieldType>, "Field type is not a supported RHIFormat" );

					constexpr ERHIFormat format = GetRHIFormatFromType<FieldType>();
					layout.Elements.EmplaceBack( a_FieldName, format, layout.Stride );
					layout.Stride += GetRHIFormatInfo( format ).Bytes();
				} 
			);

			return layout;
		}
	};

} // namespace Tridium