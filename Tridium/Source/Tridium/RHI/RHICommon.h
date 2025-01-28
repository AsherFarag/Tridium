#pragma once
#include <Tridium/Core/Config.h>
#include <Tridium/Core/Types.h>

#if CONFIG_DEBUG
	#define RHI_DEBUG_ENABLED 1
#else
	#define RHI_DEBUG_ENABLED 0
#endif // CONFIG_DEBUG


namespace Tridium {

	enum class EGraphicsAPI : uint8_t
	{
		None = 0,
		OpenGL = 1,
		DirectX11 = 2,
		DirectX12 = 3,
		Vulkan = 4
	};

	struct RHIConfig
	{
		EGraphicsAPI GraphicsAPI;
		bool UseDebug;
	};

	namespace RHI {

		bool Initialise( const RHIConfig& a_Config );
		bool Shutdown();
		bool Present();
		const char* GetGraphicsAPIName( EGraphicsAPI a_API = EGraphicsAPI::None );
		constexpr bool IsGraphicsAPISupported( EGraphicsAPI a_API );
	}

	enum class ERHIDataType : uint8_t
	{
		Float64 = 0u,
		Float32,
		Float16,
		UNorm8,
		SNorm8,
		UNorm16,
		SNorm16,
		UInt8,
		SInt8,
		UInt16,
		SInt16,
		UInt32,
		SInt32,
		COUNT,
		Unknown
	};

	//===========================
	// Format
	//===========================
    enum class EFormat : uint8_t
    {
        Unknown = 0,

        R8_UINT,
        R8_SINT,
        R8_UNORM,
        R8_SNORM,
        RG8_UINT,
        RG8_SINT,
        RG8_UNORM,
        RG8_SNORM,
        R16_UINT,
        R16_SINT,
        R16_UNORM,
        R16_SNORM,
        R16_FLOAT,
        BGRA4_UNORM,
        B5G6R5_UNORM,
        B5G5R5A1_UNORM,
        RGBA8_UINT,
        RGBA8_SINT,
        RGBA8_UNORM,
        RGBA8_SNORM,
        BGRA8_UNORM,
        SRGBA8_UNORM,
        SBGRA8_UNORM,
        R10G10B10A2_UNORM,
        R11G11B10_FLOAT,
        RG16_UINT,
        RG16_SINT,
        RG16_UNORM,
        RG16_SNORM,
        RG16_FLOAT,
        R32_UINT,
        R32_SINT,
        R32_FLOAT,
        RGBA16_UINT,
        RGBA16_SINT,
        RGBA16_FLOAT,
        RGBA16_UNORM,
        RGBA16_SNORM,
        RG32_UINT,
        RG32_SINT,
        RG32_FLOAT,
        RGB32_UINT,
        RGB32_SINT,
        RGB32_FLOAT,
        RGBA32_UINT,
        RGBA32_SINT,
        RGBA32_FLOAT,

		D16,        // Depth
		D24S8,      // Depth Stencil
		X24G8_UINT, // Depth Stencil
		D32,        // Depth
		D32S8,      // Depth Stencil
		X32G8_UINT, // Depth Stencil

		BC1_UNORM,      // DXT1
		BC1_UNORM_SRGB, // DXT1
		BC2_UNORM,      // DXT3
		BC2_UNORM_SRGB, // DXT3
		BC3_UNORM,      // DXT5
		BC3_UNORM_SRGB, // DXT5
		BC4_UNORM,      // ATI1
		BC4_SNORM,      // ATI1
		BC5_UNORM,      // ATI2
		BC5_SNORM,      // ATI2
		BC6H_UFLOAT,    // BC6H
		BC6H_SFLOAT,    // BC6H
		BC7_UNORM,      // BC7
		BC7_UNORM_SRGB, // BC7

        COUNT,
    };

	enum class EFormatKind : uint8_t
	{
		Integer,
		Normalized,
		Float,
		DepthStencil
	};

} // namespace Tridium