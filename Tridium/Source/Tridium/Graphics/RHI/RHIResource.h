#pragma once
#include "RHICommon.h"

namespace Tridium {

	// Forward declarations
	class RHIResource;
	class RHIResourceAllocator;
	using RHIResourceAllocatorRef = SharedPtr<RHIResourceAllocator>;
	// ====================

	//=====================================================================
	// ERHIResourceType
	//  The type of resource.
	//=====================================================================
	enum class ERHIResourceType : uint8_t
	{
        Sampler,
        Texture,
        ShaderModule,
        Buffer,
        VertexBuffer,
        IndexBuffer,
        ShaderInputLayout,
        PipelineState,
        CommandList,
        ResourceAllocator,
        CommandAllocator,
        COUNT
	};

	namespace Concepts {

		template<typename T>
		concept IsRHIResource = IsBaseOf<RHIResource, T>;

		template<typename T>
		concept IsRHIResourceImplemntation = IsRHIResource<T> && T::API;

	}

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
		virtual ~RHIResource() = default;

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

		// Maps the specified location on the GPU to a memory address on the CPU.
		virtual bool Map( uint64_t a_Offset = 0, int64_t a_Length = -1, ERHIMappingMode a_MappingMode = ERHIMappingMode::Default ) { return false; }
		// Unmaps the memory address on the CPU. Must be called after Map.
		virtual bool Unmap() { return false; }
		virtual bool IsMappable() const { return false; }

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
		// If 'T' is a specific implementation of a resource type, the API will also be checked.
		// E.g. if 'T' is a D3D12 texture, the API will be checked to ensure it is D3D12.
		template<typename T> requires Concepts::IsRHIResource<T>
		bool Is() const
		{
			if constexpr ( Concepts::IsRHIResourceImplemntation<T> )
			{
				return GetType() == T::Type
					&& RHI::GetRHIType() == T::API;
			}

			return GetType() == T::Type;
		}

		// Will return a shared pointer to this resource if it is the same type.
		template<typename T> requires Concepts::IsRHIResource<T>
		SharedPtr<T> As()
		{
			if ( Is<T>() )
			{
				return SharedPtrCast<T>( shared_from_this() );
			}

			return nullptr;
		}

		// Will return a shared pointer to this resource if it is the same type.
		template<typename T> requires Concepts::IsRHIResource<T>
		SharedPtr<const T> As() const
		{
			if ( Is<T>() )
			{
				return SharedPtrCast<const T>( shared_from_this() );
			}
			return nullptr;
		}

		template<typename T> requires Concepts::IsRHIResource<T>
		static T::RefType Create()
		{
			static constexpr auto Deleter = []( T* a_Resource ) { a_Resource->Release(); delete a_Resource; };
			return T::RefType( new T(), Deleter );
		}

    public:
		OpaquePtr Descriptor = nullptr;

	protected:
		template<typename T>
		const T* ParamsToDescriptor( const void* a_Params )
		{
			Descriptor = MakeUnique<T>( *reinterpret_cast<const T*>( a_Params ) );
			return reinterpret_cast<const T*>( Descriptor.Get() );
		}
	};

	//=======================================================
	// RHI Resource Descriptor
	//  A struct that describes an RHI resource.
	//=======================================================
	template<typename T> requires Concepts::IsRHIResource<T>
	struct RHIResourceDescriptor
	{
		using ResourceType = T;
		StringView Name;
		RHIResourceAllocatorRef Allocator;
	};

} // namespace Tridium

// Helper macro to define a base RHI resource type and its descriptor.
#define RHI_RESOURCE_BASE_TYPE( Name, ... )                                                               \
	class RHI##Name;                                                                                      \
	struct RHI##Name##Descriptor;                                                                         \
	using RHI##Name##Ref = SharedPtr<RHI##Name>;                                                          \
	using RHI##Name##WeakRef = SharedPtr<RHI##Name##Descriptor>;                                          \
	class RHI##Name : public ::Tridium::RHIResource                                                       \
	{                                                                                                     \
	public:                                                                                               \
		using DescriptorType = RHI##Name##Descriptor;                                                     \
		using RefType = RHI##Name##Ref;                                                                   \
		using WeakRefType = RHI##Name##WeakRef;                                                           \
        virtual ~RHI##Name() {}                                                                           \
		static constexpr ::Tridium::ERHIResourceType Type = ::Tridium::ERHIResourceType::Name;            \
		::Tridium::ERHIResourceType GetType() const override { return Type; }                             \
		const DescriptorType* GetDescriptor() const { return (const DescriptorType*)(Descriptor.Get()); } \
		__VA_ARGS__                                                                                       \
	};                                                                                                    \
	struct RHI##Name##Descriptor : public ::Tridium::RHIResourceDescriptor<RHI##Name>



// Helper macro to define the implementation of a resource type.
// For usage in an RHI implementation, such as OpenGLTexture.
#define RHI_RESOURCE_IMPLEMENTATION( GraphicsAPI )												    \
		static constexpr ::Tridium::ERHInterfaceType API = ::Tridium::ERHInterfaceType::GraphicsAPI;