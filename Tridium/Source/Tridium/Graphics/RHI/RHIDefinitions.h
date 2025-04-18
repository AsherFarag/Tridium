#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/Assert.h>
#include <Tridium/Math/Math.h>
#include <type_traits>
#include "RHIConstants.h"
#include "RHIConfig.h"
#include <Tridium/Core/Enum.h>

// Helper macro to assert that the enum size is within the bounds of the number of bits.
// This is undefined at the end of the file.
#define RHI_ENUM_SIZE_ASSERT( _Enum ) \
	static_assert( std::underlying_type_t<_Enum>(_Enum::COUNT) <= ( 1 << std::underlying_type_t<_Enum>(_Enum::NUM_BITS) ), #_Enum "::COUNT exceeds NUM_BITS" )


#define FORWARD_DECLARE_RHI_RESOURCE( _Name ) \
	struct _Name##Descriptor; \
	using _Name##Ref = SharedPtr<class _Name>; \
	using _Name##WeakRef = WeakPtr<class _Name>

// Define the log category for the RHI.
DECLARE_LOG_CATEGORY( RHI );

#ifndef RHI_ENABLE_DEV_CHECKS
	#define RHI_ENABLE_DEV_CHECKS RHI_DEBUG_ENABLED
#endif

#if RHI_ENABLE_DEV_CHECKS
	#define RHI_DEV_CHECK( _Condition, _Message ) \
			do { \
				if ( !(_Condition) ) { \
					ASSERT_LOG( false, "RHI Dev Error - " _Message ); \
				} \
			} while ( false )
#else
	#define RHI_DEV_CHECK( _Condition, _Message ) do {} while ( false )
#endif // RHI_ENABLE_DEV_CHECKS

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
	enum class ERHIFeatureSupport : uint8_t
	{
		// The RHI feature is completely unavailable at runtime.
		Unsupported,

		//// The RHI feature can be available at runtime based on hardware or driver.
		//RuntimeDependent,

		//// The RHI feature is guaranteed to be available at runtime.
		//RuntimeGuaranteed,

		TODO( "Do we only need these two?" )
		Supported
	};



	//======================================================================
	// RHI Feature
	enum class ERHIFeature : uint8_t
	{
		ComputeShaders,
		GeometryShaders,
		Tesselation,
		RayTracing,
		BindlessResources,
		COUNT,
		Invalid = ~0
	};



	//======================================================================
	// RHI Feature Info
	//  Describes a specific feature supported by the RHI device.
	struct RHIFeatureInfo
	{
		ERHIFeature Feature() const { return m_Feature; }
		StringView Name() const { return m_Name; }
		ERHIFeatureSupport Support() const { return m_Support; }

		void SetSupport( ERHIFeatureSupport a_Support ) { m_Support = a_Support; }
		RHIFeatureInfo() = default;
		RHIFeatureInfo( ERHIFeature a_Feature, StringView a_Name, ERHIFeatureSupport a_Support = ERHIFeatureSupport::Unsupported )
			: m_Feature( a_Feature ), m_Name( a_Name ), m_Support( a_Support ) {}
	private:
		ERHIFeature m_Feature = ERHIFeature::Invalid;
		StringView m_Name{ "Unknown" };
		ERHIFeatureSupport m_Support = ERHIFeatureSupport::Unsupported;
	};



	//======================================================================
	// RHI Device Features
	//  Describes the features supported by the RHI device.
	struct RHIDeviceFeatures
	{
		FixedArray<RHIFeatureInfo, uint8_t( ERHIFeature::COUNT )> Features;

		const RHIFeatureInfo& GetFeatureInfo( ERHIFeature a_Feature ) const
		{
			RHI_DEV_CHECK( uint8_t( a_Feature ) < uint8_t( ERHIFeature::COUNT ), "Invalid feature requested!" );
			return Features[static_cast<uint8_t>(a_Feature)];
		}

		RHIDeviceFeatures()
		{
		#define DEFINE_FEATURE( _Feature ) Features[static_cast<uint8_t>( ERHIFeature::_Feature )] = { ERHIFeature::_Feature, #_Feature, ERHIFeatureSupport::Unsupported };
			DEFINE_FEATURE( ComputeShaders );
			DEFINE_FEATURE( GeometryShaders );
			DEFINE_FEATURE( Tesselation );
			DEFINE_FEATURE( RayTracing );
			DEFINE_FEATURE( BindlessResources );
		#undef DEFINE_FEATURE
		}
	};



	//===========================
	// RHI Configuration
	//===========================
	struct RHIConfig
	{
		ERHInterfaceType RHIType = ERHInterfaceType::Null;
		bool UseDebug = false;
		bool CreateSwapChain = true;

		// Force single-threaded rendering.
		bool SingleThreaded = false;

		uint32_t MaxFramesInFlight = RHIConstants::MaxFrameBuffers;
	};



	//===========================
	// RHI Scissor Rect
	//  A rectangle used to clip rendering to a specific area.
	//  For example, clearing a specific area of the screen.
	//===========================
	struct RHIScissorRect
	{
		uint16_t Left;
		uint16_t Top;
		uint16_t Right;
		uint16_t Bottom;
	};



	//===========================
	// RHI Viewport
	//===========================
	struct RHIViewport
	{
		float X;        // Top-left corner of the viewport.
		float Y;        // Top-left corner of the viewport.
		float Width;    // Width of the viewport.
		float Height;   // Height of the viewport.
		float MinDepth; // Minimum depth of the viewport.
		float MaxDepth; // Maximum depth of the viewport.
	};



	//============================
	// Box
	//  Represents a 3D box in space.
	//============================
	template<typename _Scalar>
	struct TBox
	{
		using ScalarType = _Scalar;
		ScalarType MinX = 0;
		ScalarType MaxX = 0;
		ScalarType MinY = 0;
		ScalarType MaxY = 0;
		ScalarType MinZ = 0;
		ScalarType MaxZ = 1;

		constexpr TBox() noexcept = default;

		constexpr TBox( ScalarType a_MinX, ScalarType a_MaxX ) noexcept
			: MinX( a_MinX ), MaxX( a_MaxX ), MinY( 0 ), MaxY( 0 ), MinZ( 0 ), MaxZ( 1 ) 
		{}

		constexpr TBox( ScalarType a_MinX, ScalarType a_MaxX,
						  ScalarType a_MinY, ScalarType a_MaxY ) noexcept
			: MinX( a_MinX ), MaxX( a_MaxX ), MinY( a_MinY ), MaxY( a_MaxY ), MinZ( 0 ), MaxZ( 1 ) 
		{}

		constexpr TBox( ScalarType a_MinX, ScalarType a_MaxX,
						  ScalarType a_MinY, ScalarType a_MaxY,
						  ScalarType a_MinZ, ScalarType a_MaxZ ) noexcept
			: MinX( a_MinX ), MaxX( a_MaxX ), MinY( a_MinY ), MaxY( a_MaxY ), MinZ( a_MinZ ), MaxZ( a_MaxZ ) 
		{}

		template<typename T>
		constexpr TBox( const TVector3<T>& a_Min, const TVector3<T>& a_Max ) noexcept
			: MinX( static_cast<ScalarType>( a_Min.x ) ), MinY( static_cast<ScalarType>( a_Min.y ) ), MinZ( static_cast<ScalarType>( a_Min.z ) ),
			  MaxX( static_cast<ScalarType>( a_Max.x ) ), MaxY( static_cast<ScalarType>( a_Max.y ) ), MaxZ( static_cast<ScalarType>( a_Max.z ) ) 
		{}

		constexpr ScalarType Width() const noexcept { return MaxX - MinX; }
		constexpr ScalarType Height() const noexcept { return MaxY - MinY; }
		constexpr ScalarType Depth() const noexcept { return MaxZ - MinZ; }
		constexpr ScalarType Volume() const noexcept { return Width() * Height() * Depth(); }
		constexpr bool Empty() const noexcept { return Width() <= 0 || Height() <= 0 || Depth() <= 0; }
		constexpr bool Valid() const noexcept { return MaxX > MinX && MaxY > MinY && MaxZ > MinZ; }

		constexpr bool operator==( const TBox& a_Other ) const noexcept
		{
			return MinX == a_Other.MinX && MaxX == a_Other.MaxX &&
				   MinY == a_Other.MinY && MaxY == a_Other.MaxY &&
				   MinZ == a_Other.MinZ && MaxZ == a_Other.MaxZ;
		}
	};
	using Box = TBox<uint32_t>;



	//===========================
	// RHI Clear Flags
	//  Used to specify which buffers to clear.
	//===========================
	enum class ERHIClearFlags : uint8_t
	{
		Color        = 1 << 0,
		Depth        = 1 << 1,
		Stencil      = 1 << 2,
		DepthStencil = Depth | Stencil,
		ColorDepth   = Color | Depth,
		ColorStencil = Color | Stencil,
		All          = Color | Depth | Stencil,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( ERHIClearFlags );



	//===========================
	// RHI Clear Value
	//===========================
	union RHIClearValue
	{
		constexpr explicit RHIClearValue() noexcept : Color( 0.0f, 0.0f, 0.0f, 1.0f ) {}
		constexpr explicit RHIClearValue( const Color& a_Color ) noexcept : Color( a_Color ) {}
		constexpr explicit RHIClearValue( float a_Depth, uint8_t a_Stencil ) noexcept : Depth( a_Depth ), Stencil( a_Stencil ) {}

		Color Color;
		struct
		{
			float Depth;
			uint8_t Stencil;
		};
	};

	//===========================
	// RHI Object Interface
	//===========================
	class IRHIObject
	{
	public:
		virtual ~IRHIObject() = default;
	};


	
	//===========================
	// RHI Shading Path
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
	// RHI Usage Hint
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



	//======================================================================
	// RHI Bind Flags
	//  Describes which parts of a pipeline the resource can be bound to.
	//======================================================================
	enum class ERHIBindFlags : uint32_t
	{
		None = 0,
		// The buffer can be used as a vertex buffer.
		VertexBuffer = 1 << 0,
		// The buffer can be used as an index buffer.
		IndexBuffer = 1 << 1,
		// The buffer can be used as a uniform buffer.
		// NOTE: This flag cannot be combined with any other flags.
		UniformBuffer = 1 << 2,
		// The buffer or texture can be used as a shader resource.
		ShaderResource = 1 << 3,
		// The buffer or texture can be used as an unordered access view
		UnorderedAccess = 1 << 4,
		// The texture can be used as a render target.
		RenderTarget = 1 << 5,
		// The texture can be used as a depth stencil target.
		DepthStencil = 1 << 6,
		// The buffer can be used as a buffer for indirect draw commands.
		IndirectArgument = 1 << 7,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( ERHIBindFlags );



	//======================================================================
	// RHI Usage
	//  Describes the usage of an RHI resource and how it can be written to.
	//======================================================================
	enum class ERHIUsage : uint8_t
	{
		// Default Resource
		// Data can be written to via Write.
		// Use for resources that are updated less than once per frame.
		// Update Scenario: < Once per frame.
		// Equivalents:
		//  - OpenGL: GL_STATIC_DRAW
		//  - D3D11: D3D11_USAGE_DEFAULT
		Default = 0,

		// Static Resource
		// Data can only be written during commit.
		// Use for resources that don't change.
		// Update Scenario: Never.
		// Equivalents:
		//  - OpenGL: GL_STATIC_DRAW
		//  - D3D11: D3D11_USAGE_IMMUTABLE
		Static,

		// Dynamic Resource
		// Data can be written to via Map/Unmap.
		// Use for resources that can be updated multiple times in a frame.
		// Update Scenario: >= Once per frame.
		// Note: Textures with this flag should NOT be upated every frame.
		// Equivalents:
		//  - OpenGL: GL_DYNAMIC_DRAW
		//  - D3D11: D3D11_USAGE_DYNAMIC
		Dynamic,
	};



	//=====================================================================
	// RHI CPU Access
	//=====================================================================
	enum class ERHICpuAccess : uint8_t
	{
		None = 0,		          // Inaccessible to CPU
		Read = 1 << 0,            // Can be mapped for reading by the CPU
		Write = 1 << 1,           // Can be mapped for writing by the CPU
		ReadWrite = Read | Write, // Can be mapped for reading and writing by the CPU
	};



	//=====================================================================
	// RHI Resource State
	//  The state of a resource indicating how it is used.
	//=====================================================================
	enum class ERHIResourceStates : uint16_t
	{
		// Common states            // D3D12_RESOURCE_STATE_                                | VK_IMAGE_LAYOUT_                  | VK_ACCESS_            
		Unknown = 0,                // n/a                                                  | n/a                               | n/a
		Common = 1 << 0,            // COMMON                                               | UNDEFINED                         | 0
		CopySource = 1 << 1,        // COPY_SOURCE                                          | TRANSFER_SRC_OPTIMAL              | TRANSFER_READ
		CopyDest = 1 << 2,          // COPY_DEST                                            | TRANSFER_DST_OPTIMAL              | TRANSFER_WRITE

		// Render target states				                             				    
		RenderTarget = 1 << 3,      // RENDER_TARGET     	                                | COLOR_ATTACHMENT_OPTIMAL      	| COLOR_ATTACHMENT_READ_BIT|COLOR_ATTACHMENT_WRITE_BIT
		DepthStencilWrite = 1 << 4, // DEPTH_WRITE                                          | DEPTH_STENCIL_ATTACHMENT_OPTIMAL	| DEPTH_STENCIL_ATTACHMENT_READ_BIT|DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
		DepthStencilRead = 1 << 5,  // DEPTH_READ        	                                | DEPTH_STENCIL_READ_ONLY_OPTIMAL	| DEPTH_STENCIL_ATTACHMENT_READ_BIT
		Present = 1 << 6,           // PRESENT                                              | PRESENT_SRC_KHR                   | MEMORY_READ_BIT

		// Shader access states
		ShaderResource = 1 << 7,    // NON_PIXEL_SHADER_RESOURCE|PIXEL_SHADER_RESOURCE      | SHADER_READ_ONLY_OPTIMAL          | SHADER_READ_BIT
		UnorderedAccess = 1 << 8,   // UNORDERED_ACCESS                                     | GENERAL                           | SHADER_READ_BIT|SHADER_WRITE_BIT
		IndirectArgument = 1 << 9,  // INDIRECT_ARGUMENT                                    | n/a                               | INDIRECT_COMMAND_READ_BIT

		// Buffers
		VertexBuffer = 1 << 10,     // VERTEX_AND_CONSTANT_BUFFER                           | n/a                               | VERTEX_ATTRIBUTE_READ_BIT
		IndexBuffer = 1 << 11,      // INDEX_BUFFER                                         | n/a                               | INDEX_READ_BIT
		ConstantBuffer = 1 << 12,   // VERTEX_AND_CONSTANT_BUFFER                           | n/a                               | CONSTANT_BUFFER_READ_BIT
	};
	ENUM_ENABLE_BITMASK_OPERATORS( ERHIResourceStates );



	//=====================================================================
	// RHI Resource State Transition Mode
	//  Specifies how an RHI command should handle the resource state,
	//  if the command requires a spcific state.
	//=====================================================================
	enum class ERHIResourceStateTransitionMode : uint8_t
	{
		// The resource state is unknown to the RHI. 
		// All state transitions are expected to be handled outside of the RHI.
		// Note: Use 'None' instead.
		Unknown = 0,

		// Perform a state transition on the resource to the state required by the RHI command.
		// Note: Automatic state transitions are NOT thread-safe.
		//		 If the resource is used in multiple threads, use 'None' or 'Validate' instead.
		Transition,

		// Performs no state transitions on the resource, 
		// but does verify that the resource is in the correct state.
		Validate,

		// Performs no state transitions on the resource and does not verify that the resource is in the correct state.
		// Use if resource state transitions are handled outside of the RHI.
		// Note: In debug mode, this will still validate the resource.
		None = IF_RHI_DEBUG_ELSE( Validate, Unknown ),
	};



	//=====================================================================
	// RHI Descriptor Heap Type
	//  The type of resource that the allocator is managing.
	enum class ERHIDescriptorHeapType : uint8_t
	{
		Unknown = 0,
		RenderResource, // Resources such as textures, buffers, etc.
		Sampler,
		RenderTarget,
		DepthStencil,
	};



	//=====================================================================
	// RHI Descriptor Handle
	//  A handle to a descriptor in a descriptor heap.
	struct RHIDescriptorHandle
	{
		const ERHIDescriptorHeapType Type;
		const uint32_t Index;

		constexpr bool Valid() const { return Type != ERHIDescriptorHeapType::Unknown && Index != 0xFFFFFFFFu; }
		explicit constexpr RHIDescriptorHandle( ERHIDescriptorHeapType a_Type, uint32_t a_Index ) : Type( a_Type ), Index( a_Index ) {}
		// Returns an invalid descriptor handle.
		explicit constexpr RHIDescriptorHandle() : Type( ERHIDescriptorHeapType::Unknown ), Index( 0xFFFFFFFFu ) {}
	};



	//======================================================================
	// RHI Command Queue Type
	//  Used to specify the type of command queue a command list will be submitted to.
	enum class ERHICommandQueueType : uint8_t
	{
		Graphics,
		Compute,
		Copy,
		COUNT
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
		SrcColor,
		OneMinusSrcColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstColor,
		OneMinusDstColor,
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
	// Color Write Mask
	//===========================
	enum class EColorWriteMask : uint8_t
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
	// RHI Fence Value
	//  A monotonically increasing value associated with fence signaling.
	constexpr uint64_t InvalidRHIFenceValue = ~0ull;
	//==========================================================



	//==========================================================
	// RHI Fence Type
	//==========================================================
	enum class ERHIFenceType : uint8_t
	{
		// Used for:
		// - signaling the fence from the GPU
		// - waiting for the fence on the CPU
		CPUWaitOnly = 0,

		// Used for:
		// - signaling the fence from the GPU
		// - waiting for the fence on the GPU and CPU
		General = 1,
	};



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
	//==========================================================
	enum class ERHITextureDimension : uint8_t
	{
		Unknown,
		Texture1D,
		Texture2D,
		Texture3D,
		TextureCube,
		Texture1DArray,
		Texture2DArray,
		TextureCubeArray,
		COUNT,
		NUM_BITS = 3,
	};
	RHI_ENUM_SIZE_ASSERT( ERHITextureDimension );



	//==========================================================
	// RHI Format
	//  Describes the format of each pixel/element in a texture or buffer.
	//==========================================================
	enum class ERHIFormat : uint8_t
	{
		Unknown = 0, 

		// 8-bit Unsigned-Normalized
		R8_UNORM,        // DXGI_FORMAT_R8_UNORM / GL_R8 / VK_FORMAT_R8_UNORM
		RG8_UNORM,       // DXGI_FORMAT_R8G8_UNORM / GL_RG8 / VK_FORMAT_R8G8_UNORM
		RGB8_UNORM,      // DXGI_FORMAT_R8G8B8_UNORM / GL_RGB8 / VK_FORMAT_R8G8B8_UNORM
		RGBA8_UNORM,     // DXGI_FORMAT_R8G8B8A8_UNORM / GL_RGBA8 / VK_FORMAT_R8G8B8A8_UNORM

		// 8-bit Float	
		R8_FLOAT,        // DXGI_FORMAT_R8_FLOAT / GL_R8F / VK_FORMAT_R8_SFLOAT
		RG8_FLOAT,       // DXGI_FORMAT_R8G8_FLOAT / GL_RG8F / VK_FORMAT_R8G8_SFLOAT
		RGB8_FLOAT,      // DXGI_FORMAT_R8G8B8_FLOAT / GL_RGB8F / VK_FORMAT_R8G8B8_SFLOAT
		RGBA8_FLOAT,     // DXGI_FORMAT_R8G8B8A8_FLOAT / GL_RGBA8F / VK_FORMAT_R8G8B8A8_SFLOAT

		// 8-bit Signed-Integer
		R8_SINT,         // DXGI_FORMAT_R8_SINT / GL_R8I / VK_FORMAT_R8_SINT
		RG8_SINT,        // DXGI_FORMAT_R8G8_SINT / GL_RG8I / VK_FORMAT_R8G8_SINT
		RGB8_SINT,       // DXGI_FORMAT_R8G8B8_SINT / GL_RGB8I / VK_FORMAT_R8G8B8_SINT
		RGBA8_SINT,      // DXGI_FORMAT_R8G8B8A8_SINT / GL_RGBA8I / VK_FORMAT_R8G8B8A8_SINT

		// 8-bit Unsigned-Integer
		R8_UINT,         // DXGI_FORMAT_R8_UINT / GL_R8UI / VK_FORMAT_R8_UINT
		RG8_UINT,        // DXGI_FORMAT_R8G8_UINT / GL_RG8UI / VK_FORMAT_R8G8_UINT
		RGB8_UINT,       // DXGI_FORMAT_R8G8B8_UINT / GL_RGB8UI / VK_FORMAT_R8G8B8_UINT
		RGBA8_UINT,      // DXGI_FORMAT_R8G8B8A8_UINT / GL_RGBA8UI / VK_FORMAT_R8G8B8A8_UINT

		// 16-bit Unsigned-Normalized
		R16_UNORM,       // DXGI_FORMAT_R16_UNORM / GL_R16 / VK_FORMAT_R16_UNORM
		RG16_UNORM,      // DXGI_FORMAT_R16G16_UNORM / GL_RG16 / VK_FORMAT_R16G16_UNORM
		RGB16_UNORM,     // DXGI_FORMAT_R16G16B16_UNORM / GL_RGB16 / VK_FORMAT_R16G16B16_UNORM
		RGBA16_UNORM,    // DXGI_FORMAT_R16G16B16A16_UNORM / GL_RGBA16 / VK_FORMAT_R16G16B16A16_UNORM

		// 16-bit Float
		R16_FLOAT,       // DXGI_FORMAT_R16_FLOAT / GL_R16F / VK_FORMAT_R16_SFLOAT
		RG16_FLOAT,      // DXGI_FORMAT_R16G16_FLOAT / GL_RG16F / VK_FORMAT_R16G16_SFLOAT
		RGB16_FLOAT,     // DXGI_FORMAT_R16G16B16_FLOAT / GL_RGB16F / VK_FORMAT_R16G16B16_SFLOAT
		RGBA16_FLOAT,    // DXGI_FORMAT_R16G16B16A16_FLOAT / GL_RGBA16F / VK_FORMAT_R16G16B16A16_SFLOAT

		// 16-bit Signed-Integer
		R16_SINT,        // DXGI_FORMAT_R16_SINT / GL_R16I / VK_FORMAT_R16_SINT
		RG16_SINT,       // DXGI_FORMAT_R16G16_SINT / GL_RG16I / VK_FORMAT_R16G16_SINT
		RGB16_SINT,      // DXGI_FORMAT_R16G16B16_SINT / GL_RGB16I / VK_FORMAT_R16G16B16_SINT
		RGBA16_SINT,     // DXGI_FORMAT_R16G16B16A16_SINT / GL_RGBA16I / VK_FORMAT_R16G16B16A16_SINT

		// 16-bit Unsigned-Integer
		R16_UINT,        // DXGI_FORMAT_R16_UINT / GL_R16UI / VK_FORMAT_R16_UINT
		RG16_UINT,       // DXGI_FORMAT_R16G16_UINT / GL_RG16UI / VK_FORMAT_R16G16_UINT
		RGB16_UINT,      // DXGI_FORMAT_R16G16B16_UINT / GL_RGB16UI / VK_FORMAT_R16G16B16_UINT
		RGBA16_UINT,     // DXGI_FORMAT_R16G16B16A16_UINT / GL_RGBA16UI / VK_FORMAT_R16G16B16A16_UINT

		// 32-bit Unsigned-Normalized
		R32_UNORM,       // DXGI_FORMAT_R32_UNORM / GL_R32 / VK_FORMAT_R32_UINT
		RG32_UNORM,      // DXGI_FORMAT_R32G32_UNORM / GL_RG32 / VK_FORMAT_R32G32_UINT
		RGB32_UNORM,     // DXGI_FORMAT_R32G32B32_UNORM / GL_RGB32 / VK_FORMAT_R32G32B32_UINT
		RGBA32_UNORM,    // DXGI_FORMAT_R32G32B32A32_UNORM / GL_RGBA32 / VK_FORMAT_R32G32B32A32_UINT

		// 32-bit Float
		R32_FLOAT,      // DXGI_FORMAT_R32_FLOAT / GL_R32F / VK_FORMAT_R32_SFLOAT
		RG32_FLOAT,     // DXGI_FORMAT_R32G32_FLOAT / GL_RG32F / VK_FORMAT_R32G32_SFLOAT
		RGB32_FLOAT,    // DXGI_FORMAT_R32G32B32_FLOAT / GL_RGB32F / VK_FORMAT_R32G32B32_SFLOAT
		RGBA32_FLOAT,   // DXGI_FORMAT_R32G32B32A32_FLOAT / GL_RGBA32F / VK_FORMAT_R32G32B32A32_SFLOAT

		// 32-bit Signed-Integer
		R32_SINT,      // DXGI_FORMAT_R32_SINT / GL_R32I / VK_FORMAT_R32_SINT
		RG32_SINT,     // DXGI_FORMAT_R32G32_SINT / GL_RG32I / VK_FORMAT_R32G32_SINT
		RGB32_SINT,    // DXGI_FORMAT_R32G32B32_SINT / GL_RGB32I / VK_FORMAT_R32G32B32_SINT
		RGBA32_SINT,   // DXGI_FORMAT_R32G32B32A32_SINT / GL_RGBA32I / VK_FORMAT_R32G32B32A32_SINT

		// 32-bit Unsigned-Integer
		R32_UINT,      // DXGI_FORMAT_R32_UINT / GL_R32UI / VK_FORMAT_R32_UINT
		RG32_UINT,     // DXGI_FORMAT_R32G32_UINT / GL_RG32UI / VK_FORMAT_R32G32_UINT
		RGB32_UINT,    // DXGI_FORMAT_R32G32B32_UINT / GL_RGB32UI / VK_FORMAT_R32G32B32_UINT
		RGBA32_UINT,   // DXGI_FORMAT_R32G32B32A32_UINT / GL_RGBA32UI / VK_FORMAT_R32G32B32A32_UINT

		// SRGB Formats
		SRGBA8_UNORM,   // DXGI_FORMAT_B8G8R8A8_UNORM_SRGB / GL_SRGB8_ALPHA8 / VK_FORMAT_B8G8R8A8_SRGB

		// Depth-Stencil Formats
		D16_UNORM,         // DXGI_FORMAT_D16_UNORM / GL_DEPTH_COMPONENT16 / VK_FORMAT_D16_UNORM
		D32_FLOAT,         // DXGI_FORMAT_D32_FLOAT / GL_DEPTH_COMPONENT32F / VK_FORMAT_D32_SFLOAT
		D24_UNORM_S8_UINT, // DXGI_FORMAT_D24_UNORM_S8_UINT / GL_DEPTH24_STENCIL8 / VK_FORMAT_D24_UNORM_S8_UINT

		// Compressed Formats (BC / DXT)
		BC1_UNORM,     // DXGI_FORMAT_BC1_UNORM / GL_COMPRESSED_RGBA_S3TC_DXT1_EXT / VK_FORMAT_BC1_RGB_UNORM_BLOCK
		BC3_UNORM,     // DXGI_FORMAT_BC3_UNORM / GL_COMPRESSED_RGBA_S3TC_DXT5_EXT / VK_FORMAT_BC3_UNORM_BLOCK
		BC4_UNORM,     // DXGI_FORMAT_BC4_UNORM / GL_COMPRESSED_RED_RGTC1 / VK_FORMAT_BC4_UNORM_BLOCK
		BC5_UNORM,     // DXGI_FORMAT_BC5_UNORM / GL_COMPRESSED_RG_RGTC2 / VK_FORMAT_BC5_UNORM_BLOCK
		BC7_UNORM,     // DXGI_FORMAT_BC7_UNORM / GL_COMPRESSED_RGBA_BPTC_UNORM_ARB / VK_FORMAT_BC7_UNORM_BLOCK

		COUNT,
		NUM_BITS = 7,
	};
	RHI_ENUM_SIZE_ASSERT( ERHIFormat );



	//==========================================================
	// RHI Format Kind
	//  The 'kind' of format each component of the format is.
	//  E.g. RGBA32I format kind is Int.
	enum class ERHIFormatKind : uint8_t
	{
		Int,
		Float,
		Normalized,
		DepthStencil
	};



	//==========================================================
	// RHI Format Info
	//  Metadata about an RHI format.
	struct RHIFormatInfo
	{
		StringView Name;
		ERHIFormat Format;
		ERHIFormatKind Kind;
		uint8_t BytesPerBlock; // Number of bytes per block.  Note: If the format is not compressed, this is the same as BytesPerPixel.
		uint8_t Blocks;        // Number of blocks per pixel. Note: Only for compressed formats. Default is 1.
		bool HasRed : 1;
		bool HasGreen : 1;
		bool HasBlue : 1;
		bool HasAlpha : 1;
		bool HasDepth : 1;
		bool HasStencil : 1;
		bool IsSigned : 1;
		bool IsSRGB : 1;

		constexpr uint32_t Bytes() const noexcept { return BytesPerBlock * Blocks; }
	};


	// Returns the RHIFormatInfo for a given format.
	static constexpr const RHIFormatInfo& GetRHIFormatInfo( ERHIFormat a_Format )
	{
		using enum ERHIFormat;
		using enum ERHIFormatKind;

		constexpr RHIFormatInfo s_FormatInfos[size_t( ERHIFormat::COUNT )] =
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

		return s_FormatInfos[size_t( a_Format )];
	}



	//===========================================================
	// RHI Vertex Element Formats
	//  A collection of valid RHI vertex element formats.
	namespace RHIVertexElementFormats {
		// Halfs
		inline constexpr ERHIFormat Half1 = ERHIFormat::R16_FLOAT;
		inline constexpr ERHIFormat Half2 = ERHIFormat::RG16_FLOAT;
		inline constexpr ERHIFormat Half3 = ERHIFormat::RGB16_FLOAT;
		inline constexpr ERHIFormat Half4 = ERHIFormat::RGBA16_FLOAT;
		// Floats
		inline constexpr ERHIFormat Float1 = ERHIFormat::R32_FLOAT;
		inline constexpr ERHIFormat Float2 = ERHIFormat::RG32_FLOAT;
		inline constexpr ERHIFormat Float3 = ERHIFormat::RGB32_FLOAT;
		inline constexpr ERHIFormat Float4 = ERHIFormat::RGBA32_FLOAT;
		// Short Integers
		inline constexpr ERHIFormat Short1 = ERHIFormat::R16_SINT;
		inline constexpr ERHIFormat Short2 = ERHIFormat::RG16_SINT;
		inline constexpr ERHIFormat Short3 = ERHIFormat::RGB16_SINT;
		inline constexpr ERHIFormat Short4 = ERHIFormat::RGBA16_SINT;
		// Unsigned Short Integers
		inline constexpr ERHIFormat UShort1 = ERHIFormat::R16_UINT;
		inline constexpr ERHIFormat UShort2 = ERHIFormat::RG16_UINT;
		inline constexpr ERHIFormat UShort3 = ERHIFormat::RGB16_UINT;
		inline constexpr ERHIFormat UShort4 = ERHIFormat::RGBA16_UINT;
		// Integers
		inline constexpr ERHIFormat Int1 = ERHIFormat::R32_SINT;
		inline constexpr ERHIFormat Int2 = ERHIFormat::RG32_SINT;
		inline constexpr ERHIFormat Int3 = ERHIFormat::RGB32_SINT;
		inline constexpr ERHIFormat Int4 = ERHIFormat::RGBA32_SINT;
		// Unsigned Integers
		inline constexpr ERHIFormat UInt1 = ERHIFormat::R32_UINT;
		inline constexpr ERHIFormat UInt2 = ERHIFormat::RG32_UINT;
		inline constexpr ERHIFormat UInt3 = ERHIFormat::RGB32_UINT;
		inline constexpr ERHIFormat UInt4 = ERHIFormat::RGBA32_UINT;
		// Colors
		inline constexpr ERHIFormat Color = ERHIFormat::RGBA8_UNORM;
		inline constexpr ERHIFormat ColorSRGB = ERHIFormat::SRGBA8_UNORM;
	}



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

	// Get the ERHIFormat for a given type.
	// E.g. GetRHIFormatFromType<float>() returns ERHIFormat::Float1.
	// Note: An existing specialization of this template must exist for the given type.
	template<typename T>
	constexpr ERHIFormat GetRHIFormatFromType();

	template<typename T>
	concept IsRHIFormat = requires { GetRHIFormatFromType<T>(); };

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

	template<> constexpr ERHIFormat GetRHIFormatFromType<uint8_t>()            { return ERHIFormat::R8_UINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<uint16_t>()           { return ERHIFormat::R16_UINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<uint32_t>()           { return ERHIFormat::R32_UINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<int8_t>()             { return ERHIFormat::R8_SINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<int16_t>()            { return ERHIFormat::R16_SINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<int32_t>()            { return ERHIFormat::R32_SINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<float32_t>()          { return ERHIFormat::R32_FLOAT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<Vector2>()            { return ERHIFormat::RG32_FLOAT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<Vector3>()            { return ERHIFormat::RGB32_FLOAT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<Vector4>()            { return ERHIFormat::RGBA32_FLOAT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<Color>()              { return ERHIFormat::RGBA8_UNORM; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<TVector4<Byte>>()     { return ERHIFormat::RGBA8_UNORM; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<TVector2<int16_t>>()  { return ERHIFormat::RG16_SINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<TVector4<int16_t>>()  { return ERHIFormat::RGBA16_SINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<TVector2<uint16_t>>() { return ERHIFormat::RG16_UINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<TVector4<uint16_t>>() { return ERHIFormat::RGBA16_UINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<iVector2>()           { return ERHIFormat::RG32_SINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<iVector3>()           { return ERHIFormat::RGB32_SINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<iVector4>()           { return ERHIFormat::RGBA32_SINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<uVector2>()           { return ERHIFormat::RG32_UINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<uVector3>()           { return ERHIFormat::RGB32_UINT; }
	template<> constexpr ERHIFormat GetRHIFormatFromType<uVector4>()           { return ERHIFormat::RGBA32_UINT; }

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