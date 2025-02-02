#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/Assert.h>
#include <type_traits>
#include <Tridium/Math/Math.h>

// Helper macro to assert that the enum size is within the bounds of the number of bits
// This is undefined at the end of the file
#define ENUM_SIZE_ASSERT( _Enum ) \
	static_assert( std::underlying_type_t<_Enum>(_Enum::COUNT) <= ( 1 << std::underlying_type_t<_Enum>(_Enum::NUM_BITS) ), #_Enum "::COUNT exceeds NUM_BITS" )

namespace Tridium {

	//====================================
	// Rendering Hardware Interface Type
	//====================================
	enum class ERHInterfaceType : uint8_t
	{
		Null,
		OpenGL,
		DirectX11,
		DirectX12,
		Vulkan,
		Metal,
	};

	//===========================
	// RHI Feature Support
	//===========================
	enum class ERHIFeatureSupport : uint8_t
	{
		// The RHI feature is completely unavailable at runtime
		Unsupported,

		// The RHI feature can be available at runtime based on hardware or driver
		RuntimeDependent,

		// The RHI feature is guaranteed to be available at runtime.
		RuntimeGuaranteed,

		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ERHIFeatureSupport );

	//===========================
	// RHI Configuration
	//===========================
	struct RHIConfig
	{
		ERHInterfaceType RHIType = ERHInterfaceType::Null;
		bool UseDebug = false;
	};

	//===========================
	// Shading Path
	//===========================
	enum class ERHIShadingPath : uint8_t
	{
		Deferred,
		Forward,
		COUNT,
		NUM_BITS = 1,
	};
	ENUM_SIZE_ASSERT( ERHIShadingPath );

	//===========================
	// Sampler Filter
	//===========================
	enum class ESamplerFilter : uint8_t
	{
		Point,
		Bilinear,
		Trilinear,
		AnisotropicPoint,
		AnisotropicLinear,
		COUNT,
		NUM_BITS = 3,
	};
	ENUM_SIZE_ASSERT( ESamplerFilter );

	//===========================
	// Sampler Address Mode
	//===========================
	enum class ESamplerAddressMode : uint8_t
	{
		Repeat,
		Mirror,
		Clamp,

		// WARNING: Not supported on all platforms
		Border,

		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ESamplerAddressMode );

	//===========================
	// Sampler Comparison Function
	//===========================
	enum class ESamplerCompareFunction : uint8_t
	{
		Never,
		Less,
		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ESamplerCompareFunction );

	//===========================
	// Comparison Functions
	//===========================
	enum class ECompareFunction : uint8_t
	{
		Never,         // False
		Less,          // <
		Equal,         // ==
		LessEqual,	   // <=
		Greater,	   // >
		NotEqual,	   // !=
		GreaterEqual,  // >=
		Always,		   // True
		COUNT,
		NUM_BITS = 3,
	};
	ENUM_SIZE_ASSERT( ECompareFunction );

	//===========================
	// Rasterizer Fill Mode
	//===========================
	enum class ERasterizerFillMode : uint8_t
	{
		Point,
		Wireframe,
		Solid,
		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ERasterizerFillMode );

	//===========================
	// Rasterizer Cull Mode
	//===========================
	enum class ERasterizerCullMode : uint8_t
	{
		None,
		Front,
		Back,
		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ERasterizerCullMode );

	//===========================
	// Rasterizer Depth Clip Mode
	//===========================
	enum class ERasterizerDepthClipMode : uint8_t
	{
		Clip,
		Clamp,
		COUNT,
		NUM_BITS = 1,
	};
	ENUM_SIZE_ASSERT( ERasterizerDepthClipMode );


	//===========================
	// Colour Write Mask
	//===========================
	enum class EColourWriteMask : uint8_t
	{
		Red = 0x01,
		Green = 0x02,
		Blue = 0x04,
		Alpha = 0x08,

		None = 0x00,
		RG = Red | Green,
		RGB = Red | Green | Blue,
		RGBA = Red | Green | Blue | Alpha,
		BA = Blue | Alpha,
	};

	//====================================
	// RHI Data Type
	//====================================
	enum class ERHIDataType : uint8_t
	{
		Unknown,
		UInt8,
		UInt16,
		UInt32,
		UInt64,
		Int8,
		Int16,
		Int32,
		Int64,
		Float16,
		Float32,
		Float64,
		COUNT,
		NUM_BITS = 4,
	};
	ENUM_SIZE_ASSERT( ERHIDataType );

	// Returns the number of bytes for a given RHI Data Type.
	// Returns 0 if the type is unknown.
	// E.g. A float is 4 bytes, so 4 is returned.
	constexpr uint32_t GetRHIDataTypeSize( ERHIDataType a_Type )
	{
		switch ( a_Type )
		{
			using enum ERHIDataType;
			case UInt8: return 1;
			case UInt16: return 2;
			case UInt32: return 4;
			case UInt64: return 8;
			case Int8: return 1;
			case Int16: return 2;
			case Int32: return 4;
			case Int64: return 8;
			case Float16: return 2;
			case Float32: return 4;
			case Float64: return 8;
		}
		ASSERT( false );
		return 0;
	}

	template<typename T>
	constexpr ERHIDataType GetRHIDataType()
	{
		return ERHIDataType::Unknown;
	}

	//==========================================================
	// RHI Vertex Element Type
	//  The first byte is the ERHIDataType.
	//  The second byte is the number of components.
	//==========================================================
	struct RHIVertexElementType
	{
		union
		{
			struct
			{
				ERHIDataType Type;
				uint8_t Components;
			};
			uint16_t Value;
		};

		constexpr size_t GetSize() const
		{
			return GetRHIDataTypeSize( Type ) * Components;
		}

		static const RHIVertexElementType Unknown;
		static const RHIVertexElementType Half;
		static const RHIVertexElementType Half2;
		static const RHIVertexElementType Half3;
		static const RHIVertexElementType Half4;
		static const RHIVertexElementType Float;
		static const RHIVertexElementType Float2;
		static const RHIVertexElementType Float3;
		static const RHIVertexElementType Float4;
		static const RHIVertexElementType Int;
		static const RHIVertexElementType Int2;
		static const RHIVertexElementType Int3;
		static const RHIVertexElementType Int4;
		static const RHIVertexElementType UInt;
		static const RHIVertexElementType UInt2;
		static const RHIVertexElementType UInt3;
		static const RHIVertexElementType UInt4;
	};
	//==========================================================

	template<> constexpr ERHIDataType GetRHIDataType<float32_t>() { return ERHIDataType::Float32; }
	template<> constexpr ERHIDataType GetRHIDataType<float64_t>() { return ERHIDataType::Float64; }
	template<> constexpr ERHIDataType GetRHIDataType<int8_t>() { return ERHIDataType::Int8; }
	template<> constexpr ERHIDataType GetRHIDataType<int16_t>() { return ERHIDataType::Int16; }
	template<> constexpr ERHIDataType GetRHIDataType<int32_t>() { return ERHIDataType::Int32; }
	template<> constexpr ERHIDataType GetRHIDataType<int64_t>() { return ERHIDataType::Int64; }
	template<> constexpr ERHIDataType GetRHIDataType<uint8_t>() { return ERHIDataType::UInt8; }
	template<> constexpr ERHIDataType GetRHIDataType<uint16_t>() { return ERHIDataType::UInt16; }
	template<> constexpr ERHIDataType GetRHIDataType<uint32_t>() { return ERHIDataType::UInt32; }
	template<> constexpr ERHIDataType GetRHIDataType<uint64_t>() { return ERHIDataType::UInt64; }

} // namespace Tridium

#undef ENUM_SIZE_ASSERT