#pragma once
#include "RHICommon.h"

namespace Tridium {

	// Forward declarations
	class RHIResource;
	class RHIResourceAllocator;
	// ====================

	//=====================================================================
	// ERHIResourceType
	//  The type of resource.
	//=====================================================================
	enum class ERHIResourceType : uint8_t
	{
        Sampler = 0,
        Texture,
        Shader,
        ConstantBuffer,
        MutableBuffer,
        VertexBuffer,
        IndexBuffer,
        ShaderInputLayout,
        PipelineState,
        CommandList,
        ResourceAllocator,
        CommandAllocator,
        COUNT
	};

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

	template<typename T>
	concept IsRHIResource = std::is_base_of_v<RHIResource, T>;

	template<typename T>
	concept IsRHIResourceImplemntation = IsRHIResource<T> && T::API;

	//======================================================================================================
	// RHIResource
	//  An abstract class that represents a resource that can be committed to the GPU.
	//  Resources can be textures, buffers, samplers, etc.
	//  For graphics APIs, a specific implementation of this class will be created.
	//  E.g. OpenGLTexture -> RHITexture -> RHIResource
	//======================================================================================================
	class RHIResource : public EnableSharedFromThis<RHIResource>
	{
    public:
        NON_COPYABLE_OR_MOVABLE( RHIResource );
		RHIResource() = default;
        virtual ~RHIResource();

		// Commits the resource to the GPU.
		virtual bool Commit( const void* a_Params ) = 0;

		// Releases the GPU and CPU resources associated with this resource.
        virtual bool Release() = 0;

		// Reads data from the resource.
		virtual bool Read( Span<Byte>& o_Data, size_t a_SrcOffset = 0 ) { return false; }
		virtual bool IsReadable() const { return false; }

		// Writes data to the resource.
		virtual bool Write( const Span<const Byte>& a_Data, size_t a_DstOffset = 0 ) { return false; }
		virtual bool IsWritable() const { return false; }

		// Returns the size of the resource in bytes.
		virtual size_t GetSizeInBytes() const { return 0; }

		// Returns the type of the resource.
		virtual ERHIResourceType GetType() const = 0;

		// Returns whether the resource is valid.
		virtual bool IsValid() const = 0;

		// Gets a pointer to the native resource.
		// E.g. OpenGLTexture -> GLuint
		virtual const void* NativePtr() const = 0;

		// Gets a pointer to the native resource.
		// E.g. OpenGLTexture -> GLuint
		template<typename T>
		T NativePtrAs() const
		{
			return (T)( NativePtr() );
		}

		// Returns whether the resource is the same resource type.
		// If the resource is a specific implementation of a resource type, the API will also be checked.
		// E.g. if the 'T' is a DX12 texture, the API will be checked to ensure it is DX12.
		template<typename T> requires IsRHIResource<T>
		bool Is() const
		{
			if constexpr ( IsRHIResourceImplemntation<T> )
			{
				return GetType() == T::Type
					&& RHI::GetRHInterfaceType() == T::API;
			}
			else
			{
				return GetType() == T::Type;
			}
		}

		// Will return a shared pointer to this resource if it is the same type.
		template<typename T> requires IsRHIResource<T>
		SharedPtr<T> As()
		{
			if ( Is<T>() )
			{
				return SharedPtrCast<T>( shared_from_this() );
			}

			return nullptr;
		}

		// Will return a shared pointer to this resource if it is the same type.
		template<typename T> requires IsRHIResource<T>
		SharedPtr<const T> As() const
		{
			if ( Is<T>() )
			{
				return SharedPtrCast<const T>( shared_from_this() );
			}
			return nullptr;
		}

    public:
        const void* Descriptor = nullptr;

	protected:
		template<typename T>
		const T* ParamsToDescriptor( const void* a_Params )
		{
			T* newDescriptor = new T( *reinterpret_cast<const T*>( a_Params ) );
			Descriptor = newDescriptor;
			return newDescriptor;
		}
	};

	template<typename T> requires IsRHIResource<T>
	struct RHIResourceDescriptor
	{
		using ResourceType = T;
		const char* Name = nullptr;
		SharedPtr<RHIResourceAllocator> Allocator;
	};

} // namespace Tridium

// Helper macro to define a base RHI resource type and its descriptor.
#define RHI_RESOURCE_BASE_TYPE( Name, ... )                                                         \
	class RHI##Name;                                                                                \
	struct RHI##Name##Descriptor;                                                                   \
	using RHI##Name##Ref = SharedPtr<RHI##Name>;                                                    \
	using RHI##Name##WeakRef = SharedPtr<RHI##Name##Descriptor>;                                    \
	class RHI##Name : public ::Tridium::RHIResource                                                 \
	{                                                                                               \
	public:                                                                                         \
		using DescriptorType = RHI##Name##Descriptor;                                               \
        virtual ~RHI##Name() = default;                                                             \
		static constexpr ::Tridium::ERHIResourceType Type = ::Tridium::ERHIResourceType::Name;      \
		::Tridium::ERHIResourceType GetType() const override { return Type; }                       \
        static SharedPtr<RHI##Name> Create( const DescriptorType& a_Desc );                         \
		const DescriptorType* GetDescriptor() const { return (const DescriptorType*)Descriptor; }   \
		__VA_ARGS__                                                                                 \
	};                                                                                              \
	struct RHI##Name##Descriptor : public ::Tridium::RHIResourceDescriptor<RHI##Name>




#define RHI_RESOURCE_IMPLEMENTATION( GraphicsAPI )												    \
		static constexpr ::Tridium::ERHInterfaceType API = ::Tridium::ERHInterfaceType::GraphicsAPI;