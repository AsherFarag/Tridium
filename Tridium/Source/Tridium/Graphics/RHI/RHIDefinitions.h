#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/Assert.h>
#include <type_traits>
#include <Tridium/Math/Math.h>

// Helper macro to assert that the enum size is within the bounds of the number of bits.
// This is undefined at the end of the file.
#define RHI_ENUM_SIZE_ASSERT( _Enum ) \
	static_assert( std::underlying_type_t<_Enum>(_Enum::COUNT) <= ( 1 << std::underlying_type_t<_Enum>(_Enum::NUM_BITS) ), #_Enum "::COUNT exceeds NUM_BITS" )


#define FORWARD_DECLARE_RHI_RESOURCE( Name ) \
	struct RHI##Name##Descriptor; \
	using RHI##Name##Ref = SharedPtr<class RHI##Name>; \
	using RHI##Name##WeakRef = WeakPtr<class RHI##Name>

// Define the log category for the RHI.
DECLARE_LOG_CATEGORY( RHI );

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



	//====================================
	// RHI Vendor
	//====================================
	enum class ERHIVendor : uint8_t
	{
		Unknown,
		NVIDIA,
		AMD,
		Intel,
		ARM,
		Apple,
		Microsoft,
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
	RHI_ENUM_SIZE_ASSERT( ERHIFeatureSupport );



	//===========================
	// RHI Configuration
	//===========================
	struct RHIConfig
	{
		ERHInterfaceType RHIType = ERHInterfaceType::Null;
		bool UseDebug = false;
		bool CreateSwapChain = true;
	};



	//===========================
	// Scissor Rect
	//  A rectangle used to clip rendering to a specific area.
	//  For example, clearing a specific area of the screen.
	//===========================
	struct ScissorRect
	{
		uint16_t Left;
		uint16_t Top;
		uint16_t Right;
		uint16_t Bottom;
	};



	//===========================
	// Viewport
	//===========================
	struct Viewport
	{
		float X;        // Top-left corner of the viewport.
		float Y;        // Top-left corner of the viewport.
		float Width;    // Width of the viewport.
		float Height;   // Height of the viewport.
		float MinDepth; // Minimum depth of the viewport.
		float MaxDepth; // Maximum depth of the viewport.
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
	RHI_ENUM_SIZE_ASSERT( ERHIShadingPath );



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
	// ERHIResourceState
	//  The state of a resource indicating how it is used.
	//=====================================================================
	enum class ERHIResourceState : uint32_t
	{
		// Common states
		Undefined = 0,                  // Initial state (uninitialized resource)
		General,                        // General use
		CopySource,                     // Used for copying data as source
		CopyDest,                       // Used for copying data as destination

		// Render target states
		RenderTarget,                   // Used as a render target
		DepthStencilWrite,              // Used for writing to depth/stencil buffer
		DepthStencilReadOnly,           // Read-only depth/stencil buffer

		// Shader access states
		ShaderResource,                 // Readable in shaders (textures, buffers)
		UnorderedAccess,                // Read/write access in compute shaders
		IndirectArgument,               // Used for indirect draw/dispatch

		// Vertex and Index buffers
		VertexBuffer,                   // Bound as a vertex buffer
		IndexBuffer,                    // Bound as an index buffer
		ConstantBuffer,                 // Bound as a constant buffer

		// GPU-Only memory access
		Present,                        // Used for presenting to screen
		GPUWrite,                       // Used for GPU write operations
	};



	//=====================================================================
	// ERHIResourceAllocatorType
	//  The type of resource that the allocator is managing.
	enum class ERHIResourceAllocatorType : uint8_t
	{
		Unknown = 0,
		RenderResource, // Resources such as textures, buffers, etc.
		Sampler,
		RenderTarget,
		DepthStencil,
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
	RHI_ENUM_SIZE_ASSERT( ERHIMappingMode );



	//===========================
	// RHI Shader Type
	//===========================
	enum class ERHIShaderType : uint8_t
	{
		Unknown = 0,
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Compute,
		COUNT,
		NUM_BITS = 3,
	};
	RHI_ENUM_SIZE_ASSERT( ERHIShaderType );



	//=======================================================
	// RHI Shader Format
	enum class ERHIShaderFormat : uint8_t
	{
		Unknown = 0,
		HLSL6,
		SPIRV,	      // SPIR-V for Vulkan
		SPIRV_OpenGL, // SPIR-V for OpenGL

		// Console-Platform Specific
		HLSL6_XBOX, // HLSL for Xbox
		PSSL,       // PlayStation Shader Language

		COUNT,
		NUM_BITS = 3,
	};
	RHI_ENUM_SIZE_ASSERT( ERHIShaderFormat );

	//=======================================================
	// RHI Shader Model
	//  The version of the shader model to use.
	//  The higher the version, the more features are available.
	enum class ERHIShaderModel
	{
		Unknown,
		SM_5_0,
		SM_6_0,
		SM_6_1,
		SM_6_2,
		SM_6_3,
		SM_6_4,
		SM_6_5,
		SM_6_6,
		COUNT,
		NUM_BITS = 4,
	};
	RHI_ENUM_SIZE_ASSERT( ERHIShaderModel );



	//=======================================================
	// RHI Shader Visibility
	//  Defines the shader stages that a shader binding is visible to.
	enum class ERHIShaderVisibility : uint8_t
	{
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		All
	};
	//=======================================================



	//===========================
	// Sampler Filter
	//===========================
	enum class ERHISamplerFilter : uint8_t
	{
		Point,               // Nearest, no interpolation
		Bilinear,			 // Linear interpolation
		Trilinear, 		     // Linear interpolation with mipmapping
		AnisotropicPoint,    // Anisotropic filtering, no interpolation
		AnisotropicLinear,   // Anisotropic filtering with linear interpolation
		COUNT,
		NUM_BITS = 3,
	};
	RHI_ENUM_SIZE_ASSERT( ERHISamplerFilter );



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
	RHI_ENUM_SIZE_ASSERT( ERHISamplerAddressMode );



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
	RHI_ENUM_SIZE_ASSERT( ERHIComparison );



	//=================================
	// RHI Sampler Comparison Function
	//=================================
	using ERHISamplerComparison = ERHIComparison;



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
	RHI_ENUM_SIZE_ASSERT( ERHIRasterizerFillMode );



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
	RHI_ENUM_SIZE_ASSERT( ERHIRasterizerCullMode );



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
	RHI_ENUM_SIZE_ASSERT( ERHIRasterizerDepthClipMode );



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
	RHI_ENUM_SIZE_ASSERT( ERHIStencilOp );



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
	RHI_ENUM_SIZE_ASSERT( ERHIDepthOp );



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
	RHI_ENUM_SIZE_ASSERT( ERHIBlendOp );



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
	RHI_ENUM_SIZE_ASSERT( ERHIBlendEq );



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



	//==========================================================
	// RHI Texture Dimension
	enum class ERHITextureDimension : uint8_t
	{
		Unknown,
		Texture1D,
		Texture2D,
		Texture3D,
		TextureCube,
		TextureArray,
		COUNT,
		NUM_BITS = 3,
	};
	RHI_ENUM_SIZE_ASSERT( ERHITextureDimension );



	//==========================================================
	// RHI Texture Format
	//  Describes how each pixel in a texture is stored.
	enum class ERHITextureFormat : uint8_t
	{
		Unknown,

		// 8-bit Unorm
		R8,        // DXGI_FORMAT_R8_UNORM / GL_R8 / VK_FORMAT_R8_UNORM
		RG8,       // DXGI_FORMAT_R8G8_UNORM / GL_RG8 / VK_FORMAT_R8G8_UNORM
		RGBA8,     // DXGI_FORMAT_R8G8B8A8_UNORM / GL_RGBA8 / VK_FORMAT_R8G8B8A8_UNORM

		// 8-bit sRGB
		SRGB8,     // DXGI_FORMAT_UNKNOWN (Use SRGBA8) / GL_SRGB8 / VK_FORMAT_R8G8B8_SRGB
		SRGBA8,    // DXGI_FORMAT_R8G8B8A8_UNORM_SRGB / GL_SRGB8_ALPHA8 / VK_FORMAT_R8G8B8A8_SRGB

		// 16-bit Unorm & Float
		R16,       // DXGI_FORMAT_R16_UNORM / GL_R16 / VK_FORMAT_R16_UNORM
		RG16,      // DXGI_FORMAT_R16G16_UNORM / GL_RG16 / VK_FORMAT_R16G16_UNORM
		RGBA16,    // DXGI_FORMAT_R16G16B16A16_UNORM / GL_RGBA16 / VK_FORMAT_R16G16B16A16_UNORM
		R16F,      // DXGI_FORMAT_R16_FLOAT / GL_R16F / VK_FORMAT_R16_SFLOAT
		RG16F,     // DXGI_FORMAT_R16G16_FLOAT / GL_RG16F / VK_FORMAT_R16G16_SFLOAT
		RGBA16F,   // DXGI_FORMAT_R16G16B16A16_FLOAT / GL_RGBA16F / VK_FORMAT_R16G16B16A16_SFLOAT

		// 32-bit Float
		R32F,      // DXGI_FORMAT_R32_FLOAT / GL_R32F / VK_FORMAT_R32_SFLOAT
		RG32F,     // DXGI_FORMAT_R32G32_FLOAT / GL_RG32F / VK_FORMAT_R32G32_SFLOAT
		RGBA32F,   // DXGI_FORMAT_R32G32B32A32_FLOAT / GL_RGBA32F / VK_FORMAT_R32G32B32A32_SFLOAT

		// Integer Formats
		R16I,      // DXGI_FORMAT_R16_SINT / GL_R16I / VK_FORMAT_R16_SINT
		RG16I,     // DXGI_FORMAT_R16G16_SINT / GL_RG16I / VK_FORMAT_R16G16_SINT
		RGBA16I,   // DXGI_FORMAT_R16G16B16A16_SINT / GL_RGBA16I / VK_FORMAT_R16G16B16A16_SINT
		R32I,      // DXGI_FORMAT_R32_SINT / GL_R32I / VK_FORMAT_R32_SINT
		RG32I,     // DXGI_FORMAT_R32G32_SINT / GL_RG32I / VK_FORMAT_R32G32_SINT
		RGBA32I,   // DXGI_FORMAT_R32G32B32A32_SINT / GL_RGBA32I / VK_FORMAT_R32G32B32A32_SINT

		// Depth-Stencil Formats
		D16,       // DXGI_FORMAT_D16_UNORM / GL_DEPTH_COMPONENT16 / VK_FORMAT_D16_UNORM
		D32,       // DXGI_FORMAT_D32_FLOAT / GL_DEPTH_COMPONENT32F / VK_FORMAT_D32_SFLOAT
		D24S8,     // DXGI_FORMAT_D24_UNORM_S8_UINT / GL_DEPTH24_STENCIL8 / VK_FORMAT_D24_UNORM_S8_UINT

		// Compressed Formats (BC / DXT)
		BC1,       // DXGI_FORMAT_BC1_UNORM / GL_COMPRESSED_RGBA_S3TC_DXT1_EXT / VK_FORMAT_BC1_RGB_UNORM_BLOCK
		BC3,       // DXGI_FORMAT_BC3_UNORM / GL_COMPRESSED_RGBA_S3TC_DXT5_EXT / VK_FORMAT_BC3_UNORM_BLOCK
		BC4,       // DXGI_FORMAT_BC4_UNORM / GL_COMPRESSED_RED_RGTC1 / VK_FORMAT_BC4_UNORM_BLOCK
		BC5,       // DXGI_FORMAT_BC5_UNORM / GL_COMPRESSED_RG_RGTC2 / VK_FORMAT_BC5_UNORM_BLOCK
		BC7,       // DXGI_FORMAT_BC7_UNORM / GL_COMPRESSED_RGBA_BPTC_UNORM_ARB / VK_FORMAT_BC7_UNORM_BLOCK

		COUNT,
		NUM_BITS = 6,
	};
	RHI_ENUM_SIZE_ASSERT( ERHITextureFormat );

	// Returns the size in bytes of the given texture format.
	constexpr uint8_t GetTextureFormatSize( ERHITextureFormat a_Format );



	//====================================
	// RHI Data Type
	//  An enumeration of primitive data types that can be used in the RHI. 
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
	RHI_ENUM_SIZE_ASSERT( ERHIDataType );

	//==========================================================
	// RHI Data Type Traits
	//  Provides information about a data type, if a specialization of this template exists.
	template<typename T>
	struct RHIDataTypeTraits {};
	//==========================================================

	// Tests if a valid specialization of RHIDataTypeTraits exists for the given type.
	template<typename T>
	concept IsRHIDataType = requires { RHIDataTypeTraits<T>::Type; };

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

	// Get the ERHIDataType for a given type.
	// E.g. GetRHIDataType<float>() returns ERHIDataType::Float32.
	template<typename T> requires IsRHIDataType<T>
	constexpr ERHIDataType GetRHIDataType() { return RHIDataTypeTraits<T>::Type; }

	//==========================================================
	// RHI Tensor Type Traits
	//  Provides information about a tensor type, if a specialization of this template exists.
	//  A Tensor type can be a scalar, vector, or matrix.
	//  E.g. Valid tensor types are float, Vector3, Matrix3, etc.
	template<typename T>
	struct RHITensorTypeTraits;
	//==========================================================

	// A specialization of RHITensorTypeTraits for scalar types.
	// Scalars are single values such as float, int, etc.
	template<IsRHIDataType T>
	struct RHITensorTypeTraits<T>
	{
		static constexpr ERHIDataType ElementType = GetRHIDataType<T>();
		static constexpr uint8_t ElementCountX = 1;
		static constexpr uint8_t ElementCountY = 1;
	};

	// Tests if a valid specialization of RHITensorTypeTraits exists for the given type.
	template<typename T>
	concept IsRHITensorType = requires { RHITensorTypeTraits<T>::ElementType; };

	//==========================================================
	// RHI Tensor Data Type
	//  Specifies the type of data stored which can be a scalar, vector, or matrix.
	struct RHITensorType
	{
		ERHIDataType ElementType = ERHIDataType::Unknown;
		uint8_t ElementCountX = 1;
		uint8_t ElementCountY = 1;
		uint8_t ArrayLength = 1;

		constexpr uint32_t GetSizeInBytes() const
		{
			return GetRHIDataTypeSize( ElementType ) * ElementCountX * ElementCountY * ArrayLength;
		}

		template<typename T>
		static constexpr RHITensorType From()
		{
			RHITensorType type;
			type.ElementType = RHITensorTypeTraits<T>::ElementType;
			type.ElementCountX = RHITensorTypeTraits<T>::ElementCountX;
			type.ElementCountY = RHITensorTypeTraits<T>::ElementCountY;
			return type;
		}
	};



	//==========================================================
	// RHI Vertex Element Type
	//  Describes the type of data a vertex element can be.
	enum class ERHIVertexElementType : uint8_t
	{
		None,
		Float1,
		Float2,
		Float3,
		Float4,
		UByte4,
		Color,
		Short2,
		Short4,
		Short2N,		// 16 bit word normalized to (value/32767.0,value/32767.0,0,0,1)
		Short4N,		// 4 X 16 bit word, normalized 
		Half2,			// 16 bit float using 1 bit sign, 5 bit exponent, 10 bit mantissa 
		Half4,
		UShort2,
		UShort4,
		UShort2N,		// 16 bit word normalized to (value/65535.0,value/65535.0,0,0,1)
		UShort4N,		// 4 X 16 bit word unsigned, normalized 
		URGB10A2N,		// 10 bit r, g, b and 2 bit a normalized to (value/1023.0f, value/1023.0f, value/1023.0f, value/3.0f)
		Int1,
		Int2,
		Int3,
		Int4,
		UInt1,
		UInt2,
		UInt3,
		UInt4,
		COUNT,
		NUM_BITS = 5,
	};
	RHI_ENUM_SIZE_ASSERT( ERHIVertexElementType );

	// Get the ERHIVertexElementType for a given type.
	// E.g. GetRHIVertexElementType<float>() returns ERHIVertexElementType::Float1.
	// Note: An existing specialization of this template must exist for the given type.
	template<typename T>
	constexpr ERHIVertexElementType GetRHIVertexElementType();

	// Returns the number of bytes for a given RHI Vertex Element Type.
	// Returns 0 if the type is unknown.
	inline constexpr uint32_t GetRHIVertexElementTypeSize( ERHIVertexElementType a_Type )
	{
		switch ( a_Type )
		{
			using enum ERHIVertexElementType;
			case Float1:    return 4;
			case Float2:    return 4 * 2;
			case Float3:    return 4 * 3;
			case Float4:    return 4 * 4;
			case UByte4:    return 1 * 4;
			case Color:     return 4 * 4;
			case Short2:    return 2 * 2;
			case Short4:    return 2 * 4;
			case Short2N:   return 2 * 2;
			case Short4N:   return 2 * 4;
			case Half2:     return 2 * 2;
			case Half4:     return 2 * 4;
			case UShort2:   return 2 * 2;
			case UShort4:   return 2 * 4;
			case UShort2N:  return 2 * 2;
			case UShort4N:  return 2 * 4;
			case URGB10A2N: return 10 + 10 + 10 + 2;
			case Int1:      return 4;
			case Int2:      return 4 * 2;
			case Int3:      return 4 * 3;
			case Int4:      return 4 * 4;
			case UInt1:     return 4;
			case UInt2:     return 4 * 2;
			case UInt3:     return 4 * 3;
			case UInt4:     return 4 * 4;
			default: return 0;
		}
	}



	//==========================================================

	inline constexpr uint8_t GetTextureFormatSize( ERHITextureFormat a_Format )
	{
		switch ( a_Format )
		{
			using enum ERHITextureFormat;
			case R8:      return 1;
			case RG8:     return 2;
			case RGBA8:   return 4;
			case SRGB8:   return 3;
			case SRGBA8:  return 4;
			case R16:     return 2;
			case RG16:    return 4;
			case RGBA16:  return 8;
			case R16F:    return 2;
			case RG16F:   return 4;
			case RGBA16F: return 8;
			case R32F:    return 4;
			case RG32F:   return 8;
			case RGBA32F: return 16;
			case R16I:    return 2;
			case RG16I:   return 4;
			case RGBA16I: return 8;
			case R32I:    return 4;
			case RG32I:   return 8;
			case RGBA32I: return 16;
			case D16:     return 2;
			case D32:     return 4;
			case D24S8:   return 4;
			case BC1:     return 8;
			case BC3:     return 16;
			case BC4:     return 8;
			case BC5:     return 16;
			case BC7:     return 16;
			default:      return 0;
		}
	}

	//==========================================================

	template<> struct RHIDataTypeTraits<uint8_t>   { static constexpr ERHIDataType Type = ERHIDataType::UInt8; };
	template<> struct RHIDataTypeTraits<uint16_t>  { static constexpr ERHIDataType Type = ERHIDataType::UInt16; };
	template<> struct RHIDataTypeTraits<uint32_t>  { static constexpr ERHIDataType Type = ERHIDataType::UInt32; };
	template<> struct RHIDataTypeTraits<uint64_t>  { static constexpr ERHIDataType Type = ERHIDataType::UInt64; };
	template<> struct RHIDataTypeTraits<int8_t>    { static constexpr ERHIDataType Type = ERHIDataType::Int8; };
	template<> struct RHIDataTypeTraits<int16_t>   { static constexpr ERHIDataType Type = ERHIDataType::Int16; };
	template<> struct RHIDataTypeTraits<int32_t>   { static constexpr ERHIDataType Type = ERHIDataType::Int32; };
	template<> struct RHIDataTypeTraits<int64_t>   { static constexpr ERHIDataType Type = ERHIDataType::Int64; };
	template<> struct RHIDataTypeTraits<float32_t> { static constexpr ERHIDataType Type = ERHIDataType::Float32; };
	template<> struct RHIDataTypeTraits<float64_t> { static constexpr ERHIDataType Type = ERHIDataType::Float64; };

	//==========================================================

	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<int32_t>()            { return ERHIVertexElementType::Int1;    }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<uint32_t>()           { return ERHIVertexElementType::Int2;    }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<float32_t>()          { return ERHIVertexElementType::Float1;  }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<Vector2>()            { return ERHIVertexElementType::Float2;  }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<Vector3>()            { return ERHIVertexElementType::Float3;  }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<Vector4>()            { return ERHIVertexElementType::Float4;  }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<Color>()              { return ERHIVertexElementType::Color;   }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<TVector4<Byte>>()     { return ERHIVertexElementType::UByte4;  }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<TVector2<int16_t>>()  { return ERHIVertexElementType::Short2;  }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<TVector4<int16_t>>()  { return ERHIVertexElementType::Short4;  }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<TVector2<uint16_t>>() { return ERHIVertexElementType::UShort2; }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<TVector4<uint16_t>>() { return ERHIVertexElementType::UShort4; }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<iVector2>()           { return ERHIVertexElementType::Int2;    }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<iVector3>()           { return ERHIVertexElementType::Int3;    }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<iVector4>()           { return ERHIVertexElementType::Int4;    }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<uVector2>()           { return ERHIVertexElementType::UInt2;   }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<uVector3>()           { return ERHIVertexElementType::UInt3;   }
	template<> constexpr ERHIVertexElementType GetRHIVertexElementType<uVector4>()           { return ERHIVertexElementType::UInt4;   }

	//==========================================================

	template<typename U>
	struct RHITensorTypeTraits<TVector2<U>>
	{
		static constexpr ERHIDataType ElementType = GetRHIDataType<U>();
		static constexpr uint8_t ElementCountX = 2;
		static constexpr uint8_t ElementCountY = 1;
	};

	template<typename U>
	struct RHITensorTypeTraits<TVector3<U>>
	{
		static constexpr ERHIDataType ElementType = GetRHIDataType<U>();
		static constexpr uint8_t ElementCountX = 3;
		static constexpr uint8_t ElementCountY = 1;
	};

	template<typename U>
	struct RHITensorTypeTraits<TVector4<U>>
	{
		static constexpr ERHIDataType ElementType = GetRHIDataType<U>();
		static constexpr uint8_t ElementCountX = 4;
		static constexpr uint8_t ElementCountY = 1;
	};

	template<typename U>
	struct RHITensorTypeTraits<TMatrix2<U>>
	{
		static constexpr ERHIDataType ElementType = GetRHIDataType<U>();
		static constexpr uint8_t ElementCountX = 2;
		static constexpr uint8_t ElementCountY = 2;
	};

	template<typename U>
	struct RHITensorTypeTraits<TMatrix3<U>>
	{
		static constexpr ERHIDataType ElementType = GetRHIDataType<U>();
		static constexpr uint8_t ElementCountX = 3;
		static constexpr uint8_t ElementCountY = 3;
	};

	template<typename U>
	struct RHITensorTypeTraits<TMatrix4<U>>
	{
		static constexpr ERHIDataType ElementType = GetRHIDataType<U>();
		static constexpr uint8_t ElementCountX = 4;
		static constexpr uint8_t ElementCountY = 4;
	};

	template<>
	struct RHITensorTypeTraits<Color>
	{
		static constexpr ERHIDataType ElementType = ERHIDataType::Float32;
		static constexpr uint8_t ElementCountX = 4;
		static constexpr uint8_t ElementCountY = 1;
	};

	//==========================================================


} // namespace Tridium

#undef RHI_ENUM_SIZE_ASSERT