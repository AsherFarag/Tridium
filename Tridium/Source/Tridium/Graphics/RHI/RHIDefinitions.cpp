#include "tripch.h"
#include "RHIDefinitions.h"

namespace Tridium {

	const RHIFormatInfo& GetRHIFormatInfo( ERHIFormat a_Format )
    {
		using enum ERHIFormat;
		using enum ERHIFormatKind;

		static constexpr RHIFormatInfo s_FormatInfos[size_t( ERHIFormat::COUNT )] =
		{
		//    Name,            Format,                Kind        Bytes, Blocks,   Red,     Green,   Blue,    Alpha,   Depth,   Stencil,  Signed,   SRGB
			{ "Unknown",       Unknown,               Int,            0,      0,  false,    false,   false,   false,   false,    false,   false,   false },
											  	       		       				 									  
			// 8-bit Unsigned-Normalized	  	       		       				 									  
			{ "R8_UNORM",      R8_UNORM,              Normalized,     1,      1,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "RG8_UNORM",     RG8_UNORM,             Normalized,     2,      1,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "RGB8_UNORM",    RGB8_UNORM,            Normalized,     3,      1,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "RGBA8_UNORM",   RGBA8_UNORM,           Normalized,     4,      1,  true,     true,    true,    true,    false,    false,   false,   false },
											          
			// 8-bit Float					          
			{ "R8_FLOAT",      R8_FLOAT,              Float,          1,      1,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "RG8_FLOAT",     RG8_FLOAT,             Float,          2,      1,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "RGB8_FLOAT",    RGB8_FLOAT,            Float,          3,      1,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "RGBA8_FLOAT",   RGBA8_FLOAT,           Float,          4,      1,  true,     true,    true,    true,    false,    false,   false,   false },
											          
			// 8-bit Signed-Integer			          
			{ "R8_SINT",       R8_SINT,               Int,            1,      1,  true,     false,   false,   false,   false,    false,   true,    false },
			{ "RG8_SINT",      RG8_SINT,              Int,            2,      1,  true,     true,    false,   false,   false,    false,   true,    false },
			{ "RGB8_SINT",     RGB8_SINT,             Int,            3,      1,  true,     true,    true,    false,   false,    false,   true,    false },
			{ "RGBA8_SINT",    RGBA8_SINT,            Int,            4,      1,  true,     true,    true,    true,    false,    false,   true,    false },
											          
			// 8-bit Unsigned-Integer		          
			{ "R8_UINT",       R8_UINT,               Int,            1,      1,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "RG8_UINT",      RG8_UINT,              Int,            2,      1,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "RGB8_UINT",     RGB8_UINT,             Int,            3,      1,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "RGBA8_UINT",    RGBA8_UINT,            Int,            4,      1,  true,     true,    true,    true,    false,    false,   false,   false },
											          
			// 16-bit Unsigned-Normalized	          
			{ "R16_UNORM",     R16_UNORM,             Normalized,     2,      1,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "RG16_UNORM",    RG16_UNORM,            Normalized,     4,      1,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "RGB16_UNORM",   RGB16_UNORM,           Normalized,     6,      1,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "RGBA16_UNORM",  RGBA16_UNORM,          Normalized,     8,      1,  true,     true,    true,    true,    false,    false,   false,   false },
											          
			// 16-bit Float					          
			{ "R16_FLOAT",     R16_FLOAT,             Float,          2,      1,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "RG16_FLOAT",    RG16_FLOAT,            Float,          4,      1,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "RGB16_FLOAT",   RGB16_FLOAT,           Float,          6,      1,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "RGBA16_FLOAT",  RGBA16_FLOAT,          Float,          8,      1,  true,     true,    true,    true,    false,    false,   false,   false },
											          
			// 16-bit Signed-Integer		          
			{ "R16_SINT",      R16_SINT,              Int,            2,      1,  true,     false,   false,   false,   false,    false,   true,    false },
			{ "RG16_SINT",     RG16_SINT,             Int,            4,      1,  true,     true,    false,   false,   false,    false,   true,    false },
			{ "RGB16_SINT",    RGB16_SINT,            Int,            6,      1,  true,     true,    true,    false,   false,    false,   true,    false },
			{ "RGBA16_SINT",   RGBA16_SINT,           Int,            8,      1,  true,     true,    true,    true,    false,    false,   true,    false },
											          
			// 16-bit Unsigned-Integer		          
			{ "R16_UINT",      R16_UINT,              Int,            2,      1,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "RG16_UINT",     RG16_UINT,             Int,            4,      1,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "RGB16_UINT",    RGB16_UINT,            Int,            6,      1,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "RGBA16_UINT",   RGBA16_UINT,           Int,            8,      1,  true,     true,    true,    true,    false,    false,   false,   false },
											          
			// 32-bit Unsigned-Normalized	          
			{ "R32_UNORM",     R32_UNORM,             Normalized,     4,      1,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "RG32_UNORM",    RG32_UNORM,            Normalized,     8,      1,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "RGB32_UNORM",   RGB32_UNORM,           Normalized,    12,      1,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "RGBA32_UNORM",  RGBA32_UNORM,          Normalized,    16,      1,  true,     true,    true,    true,    false,    false,   false,   false },
											          
			// 32-bit Float					          
			{ "R32_FLOAT",     R32_FLOAT,             Float,          4,      1,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "RG32_FLOAT",    RG32_FLOAT,            Float,          8,      1,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "RGB32_FLOAT",   RGB32_FLOAT,           Float,         12,      1,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "RGBA32_FLOAT",  RGBA32_FLOAT,          Float,         16,      1,  true,     true,    true,    true,    false,    false,   false,   false },
											          
			// 32-bit Signed-Integer		          
			{ "R32_SINT",      R32_SINT,              Int,            4,      1,  true,     false,   false,   false,   false,    false,   true,    false },
			{ "RG32_SINT",     RG32_SINT,             Int,            8,      1,  true,     true,    false,   false,   false,    false,   true,    false },
			{ "RGB32_SINT",    RGB32_SINT,            Int,           12,      1,  true,     true,    true,    false,   false,    false,   true,    false },
			{ "RGBA32_SINT",   RGBA32_SINT,           Int,           16,      1,  true,     true,    true,    true,    false,    false,   true,    false },
											          
			// 32-bit Unsigned-Integer		          
			{ "R32_UINT",      R32_UINT,              Int,            4,      1,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "RG32_UINT",     RG32_UINT,             Int,            8,      1,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "RGB32_UINT",    RGB32_UINT,            Int,           12,      1,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "RGBA32_UINT",   RGBA32_UINT,           Int,           16,      1,  true,     true,    true,    true,    false,    false,   false,   false },
											          
			// SRGB Formats					          
			{ "SRGBA8_UNORM",  SRGBA8_UNORM,          Normalized,     4,      1,  true,     true,    true,    true,    false,    false,   false,   true },
											          
			// Depth Formats				          
			{ "D16_UNORM",     D16_UNORM,             DepthStencil,   2,      1,  false,    false,   false,   false,   true,     false,   false,   false },
			{ "D32_FLOAT",     D32_FLOAT,             DepthStencil,   4,      1,  false,    false,   false,   false,   true,     false,   false,   false },
			{ "D24_UNORM_S8_UINT", D24_UNORM_S8_UINT, DepthStencil,   4,      1,  false,    false,   false,   false,   true,     true,    false,   false },

			// Compressed Formats
			{ "BC1_UNORM",     BC1_UNORM,             Normalized,     8,      4,  true,     true,    true,    false,   false,    false,   false,   false },
			{ "BC3_UNORM",     BC3_UNORM,             Normalized,    16,      4,  true,     true,    true,    true,    false,    false,   false,   false },
			{ "BC4_UNORM",     BC4_UNORM,             Normalized,     8,      4,  true,     false,   false,   false,   false,    false,   false,   false },
			{ "BC5_UNORM",     BC5_UNORM,             Normalized,    16,      4,  true,     true,    false,   false,   false,    false,   false,   false },
			{ "BC7_UNORM",     BC7_UNORM,             Normalized,    16,      4,  true,     true,    true,    true,    false,    false,   false,   false },
		};

		CHECK_LOG( size_t( a_Format ) < size_t ( ERHIFormat::COUNT ), "Invalid RHI format." );
		return s_FormatInfos[size_t( a_Format )];
    }
}
