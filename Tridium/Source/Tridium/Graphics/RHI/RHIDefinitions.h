#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/Assert.h>
#include <type_traits>
#include <Tridium/Math/Math.h>

// Helper macro to assert that the enum size is within the bounds of the number of bits.
// This is undefined at the end of the file.
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
		// The RHI feature is completely unavailable at runtime.
		Unsupported,

		// The RHI feature can be available at runtime based on hardware or driver.
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



	//=====================================================================
	// ERHIUsageHint
	//  A hint to the RHI about how the resource will be used.
	//  This can be used to optimize the resource for the intended usage.
	//=====================================================================
	enum class ERHIUsageHint : uint8_t
	{
		CPUWriteNever = 0b00 << 0,
		CPUWriteFew = 0b01 << 0,
		CPUWriteOnce = CPUWriteFew,
		CPUWriteMany = 0b11 << 0,
		CPUReadNever = 0b00 << 2,
		CPUReadFew = 0b01 << 2,
		CPUReadMany = 0b11 << 2,
		GPUWriteNever = 0b00 << 4,
		GPUWriteFew = 0b01 << 4,
		GPUWriteMany = 0b11 << 4,
		GPUReadNever = 0b00 << 6,
		GPUReadFew = 0b01 << 6,
		GPUReadMany = 0b11 << 6,

		OneWriteManyDraw = CPUWriteOnce | GPUReadMany,                         // Use if the resource only uses initial data from the descriptor.
		ManyWriteManyDraw = CPUWriteMany | GPUReadMany,                        // Use if the resource is expected to be Mapped/Written to many times.
		OneWriteFewDraw = CPUWriteOnce | GPUReadFew,                           // Use for streaming resources.
		RenderTarget = CPUWriteNever | GPUWriteMany | GPUReadNever,            // Use for render target (Only valid for 2D textures).
		RWRenderTarget = CPUWriteNever | GPUWriteMany | GPUReadMany,           // Use for rw-enabled render target (Only valid for 2D textures).
		MutableBuffer = CPUWriteFew | CPUReadFew | GPUWriteMany | GPUReadMany, // Use for a simple MutableBuffer.

		Default = OneWriteManyDraw,
	};



	//=====================================================================
	// ERHIMappingMode
	//  The mode to use when mapping a resource.
	//=====================================================================
	enum class ERHIMappingMode : uint8_t
	{
		ReadOnly,
		WriteOnly,
		ReadWrite,
		COUNT,
		NUM_BITS = 2,
		Default = ReadWrite,
	};
	ENUM_SIZE_ASSERT( ERHIMappingMode );



	//===========================
	// RHI Shader Type
	//===========================
	enum class ERHIShaderType : uint8_t
	{
		Unknown = 0,
		Vertex,
		Pixel,
		Geometry,
		Compute,
		COUNT,
		NUM_BITS = 3,
	};
	ENUM_SIZE_ASSERT( ERHIShaderType );



	//===========================
	// Sampler Filter
	//===========================
	enum class ERHISamplerFilter : uint8_t
	{
		Point,
		Bilinear,
		Trilinear,
		AnisotropicPoint,
		AnisotropicLinear,
		COUNT,
		NUM_BITS = 3,
	};
	ENUM_SIZE_ASSERT( ERHISamplerFilter );



	//===========================
	// Sampler Address Mode
	//===========================
	enum class ERHISamplerAddressMode : uint8_t
	{
		Repeat,
		Mirror,
		Clamp,

		// WARNING: Not supported on all platforms
		Border,

		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ERHISamplerAddressMode );



	//=================================
	// RHI Sampler Comparison Function
	//=================================
	enum class ERHISamplerComparison : uint8_t
	{
		Never,
		Less,
		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ERHISamplerComparison );



	//===========================
	// RHI Comparison Functions
	//===========================
	enum class ERHIComparison : uint8_t
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
	ENUM_SIZE_ASSERT( ERHIComparison );



	//===========================
	// RHI Rasterizer Fill Mode
	//===========================
	enum class ERHIRasterizerFillMode : uint8_t
	{
		Point,
		Wireframe,
		Solid,
		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ERHIRasterizerFillMode );



	//===========================
	// RHI Rasterizer Cull Mode
	//===========================
	enum class ERHIRasterizerCullMode : uint8_t
	{
		None,
		Front,
		Back,
		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ERHIRasterizerCullMode );



	//===========================
	// RHI Rasterizer Depth Clip Mode
	//===========================
	enum class ERHIRasterizerDepthClipMode : uint8_t
	{
		Clip,
		Clamp,
		COUNT,
		NUM_BITS = 1,
	};
	ENUM_SIZE_ASSERT( ERHIRasterizerDepthClipMode );



	//===========================
	// RHI Stencil Operation
	//===========================
	enum class ERHIStencilOp : uint8_t
	{
		Keep,
		Zero,
		Replace,
		Invert,
		Increment,
		Decrement,
		COUNT,
		NUM_BITS = 3,
	};
	ENUM_SIZE_ASSERT( ERHIStencilOp );



	//===========================
	// RHI Depth Operation
	//===========================
	enum class ERHIDepthOp : uint8_t
	{
		Keep,
		Replace,
		COUNT,
		NUM_BITS = 2,
	};
	ENUM_SIZE_ASSERT( ERHIDepthOp );



	//===========================
	// RHI Blend Operation
	//===========================
	enum class ERHIBlendOp : uint8_t
	{
		Zero,
		One,
		SrcColour,
		OneMinusSrcColour,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstColour,
		OneMinusDstColour,
		DstAlpha,
		OneMinusDstAlpha,
		SrcAlphaSaturate,
		COUNT,
		NUM_BITS = 4,
	};
	ENUM_SIZE_ASSERT( ERHIBlendOp );



	//===========================
	// RHI Blend Equation
	//===========================
	enum class ERHIBlendEq : uint8_t
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max,
		COUNT,
		NUM_BITS = 3,
	};
	ENUM_SIZE_ASSERT( ERHIBlendEq );



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



	//==========================================================
	// RHI Fence
	//  A fence that can be used to synchronize the CPU and GPU.
	enum class RHIFence : int32_t {};
	//==========================================================



	//==========================================================
	// RHI Fence State
	//  The state of a fence.
	//==========================================================
	enum class ERHIFenceState : uint8_t
	{
		Pending,  // The fence has not been signaled yet.
		Complete, // The fence has been signaled.
		Unknown   // The state of the fence is unknown.
	};



	//==========================================================
	// RHI Texture Alignment
	//  Each Graphics API can have different defaults for texture alignment.
	//  For example, DirectX and Metal align their textures from the top-left corner.
	//  OpenGL and other API's align their textures from the bottom-left corner.
	//==========================================================
	enum class ERHITextureAlignment : uint8_t
	{
		TopLeft,
		BottomLeft,
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
	};



	//==========================================================
	// Predefined Vertex Element Types
	//==========================================================
	namespace RHIVertexElementTypes {
		static constexpr RHIVertexElementType Unknown{ ERHIDataType::Unknown, 0 };
		static constexpr RHIVertexElementType Half{ ERHIDataType::Float16, 1 };
		static constexpr RHIVertexElementType Half2{ ERHIDataType::Float16, 2 };
		static constexpr RHIVertexElementType Half3{ ERHIDataType::Float16, 3 };
		static constexpr RHIVertexElementType Half4{ ERHIDataType::Float16, 4 };
		static constexpr RHIVertexElementType Float{ ERHIDataType::Float32, 1 };
		static constexpr RHIVertexElementType Float2{ ERHIDataType::Float32, 2 };
		static constexpr RHIVertexElementType Float3{ ERHIDataType::Float32, 3 };
		static constexpr RHIVertexElementType Float4{ ERHIDataType::Float32, 4 };
		static constexpr RHIVertexElementType Int{ ERHIDataType::Int32, 1 };
		static constexpr RHIVertexElementType Int2{ ERHIDataType::Int32, 2 };
		static constexpr RHIVertexElementType Int3{ ERHIDataType::Int32, 3 };
		static constexpr RHIVertexElementType Int4{ ERHIDataType::Int32, 4 };
		static constexpr RHIVertexElementType UInt{ ERHIDataType::UInt32, 1 };
		static constexpr RHIVertexElementType UInt2{ ERHIDataType::UInt32, 2 };
		static constexpr RHIVertexElementType UInt3{ ERHIDataType::UInt32, 3 };
		static constexpr RHIVertexElementType UInt4{ ERHIDataType::UInt32, 4 };
		static constexpr RHIVertexElementType Mat2x2{ ERHIDataType::Float32, 4 };
		static constexpr RHIVertexElementType Mat3x3{ ERHIDataType::Float32, 9 };
		static constexpr RHIVertexElementType Mat4x4{ ERHIDataType::Float32, 16 };
	}

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

	//==========================================================

} // namespace Tridium

#undef ENUM_SIZE_ASSERT