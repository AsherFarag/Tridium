#pragma once
#include "RHICommon.h"

namespace Tridium {

	// Forward declarations
	class RHIResource;
	// ====================

	//=====================================================================
	// RHI Resource Type
	//  The type of resource.
	//=====================================================================
	enum class ERHIResourceType : uint8_t
	{
        Sampler,
        Texture,
        ShaderModule,
        Buffer,
        BindingLayout,
		BindingSet,
        GraphicsPipelineState,
		ComputePipelineState,
        CommandList,
        CommandAllocator,
		SwapChain,
		Fence,
        COUNT,
		Unknown = 0xFF,
	};

	static constexpr StringView RHIResourceTypeToString( ERHIResourceType a_Type )
	{
		switch ( a_Type )
		{
			case ERHIResourceType::Sampler:               return "Sampler";
			case ERHIResourceType::Texture:               return "Texture";
			case ERHIResourceType::ShaderModule:          return "ShaderModule";
			case ERHIResourceType::Buffer:                return "Buffer";
			case ERHIResourceType::BindingLayout:         return "BindingLayout";
			case ERHIResourceType::BindingSet:            return "BindingSet";
			case ERHIResourceType::GraphicsPipelineState: return "GraphicsPipelineState";
			case ERHIResourceType::ComputePipelineState:  return "ComputePipelineState";
			case ERHIResourceType::CommandList:           return "CommandList";
			case ERHIResourceType::CommandAllocator:      return "CommandAllocator";
			case ERHIResourceType::SwapChain:             return "SwapChain";
			case ERHIResourceType::Fence:                 return "Fence";
			case ERHIResourceType::Unknown:               return "Unknown";
			default:                                      return "<INVALID>";
		}
	}

	static constexpr ERHIResourceType RHIResourceTypeFromString( StringView a_Type )
	{
		switch ( Hashing::Hash( ReinterpretCast<const uint8_t*>(a_Type.data()), a_Type.size() ) )
		{
			case "Sampler"_H:               return ERHIResourceType::Sampler;
			case "Texture"_H:               return ERHIResourceType::Texture;
			case "ShaderModule"_H:          return ERHIResourceType::ShaderModule;
			case "Buffer"_H:                return ERHIResourceType::Buffer;
			case "BindingLayout"_H:         return ERHIResourceType::BindingLayout;
			case "BindingSet"_H:            return ERHIResourceType::BindingSet;
			case "GraphicsPipelineState"_H: return ERHIResourceType::GraphicsPipelineState;
			case "ComputePipelineState"_H:  return ERHIResourceType::ComputePipelineState;
			case "CommandList"_H:           return ERHIResourceType::CommandList;
			case "CommandAllocator"_H:      return ERHIResourceType::CommandAllocator;
			case "SwapChain"_H:             return ERHIResourceType::SwapChain;
			case "Fence"_H:                 return ERHIResourceType::Fence;
		}

		return ERHIResourceType::Unknown;
	}

	//=====================================================================

	namespace Concepts {

		template<typename T>
		concept IsRHIResource = IsBaseOf<RHIResource, T>;

		template<typename T>
		concept IsRHIResourceImplemntation =
			IsRHIResource<T>
			&& requires ( T ) { T::API; };

	} // namespace Concepts


	// Strong reference-counting pointer to an RHI resource.
	using RHIResourceRef = SharedPtr<class RHIResource>;
	// Weak reference-counting pointer to an RHI resource.
	using RHIResourceWeakRef = WeakPtr<class RHIResource>;

	//======================================================================================================
	// RHI Resource
	//  An abstract class that represents a resource that can be committed to the GPU.
	//  Resources can be textures, buffers, samplers, etc.
	//  For graphics APIs, a specific implementation of this class will be created.
	//  E.g. RHITexture_OpenGLImpl -> RHITexture -> RHIResource
	//======================================================================================================
	class RHIResource : public IRHIObject, public EnableSharedFromThis<RHIResource>
	{
    public:
        NON_COPYABLE_OR_MOVABLE( RHIResource );

		// Releases the GPU and CPU resources associated with this resource.
		virtual bool Release() = 0;

		// Returns the size, in bytes, of the allocated memory for the resource on the GPU.
		// Returns 0 if the size is unknown.
		virtual size_t GetSizeInBytes() const { return 0; }

		// Returns the type of the resource.
		virtual ERHIResourceType GetType() const = 0;

		// Returns whether this resource is in a usable state.
		virtual bool IsValid() const = 0;

		// Gets a pointer to the native resource.
		// E.g. RHITexture_OpenGLImpl -> GLuint, RHITexture_D3D12Impl -> ID3D12Resource
		virtual const void* NativePtr() const = 0;

		// Gets a pointer to the native resource and casts it to the specified type.
		// E.g. RHITexture_OpenGLImpl->NativePtrAs<GLuint>() -> GLuint*,
		//      RHITexture_D3D12Impl->NativePtrAs<ID3D12Resource>() -> ID3D12Resource*
		template<typename T>
		T* NativePtrAs() const
		{
			return (T*)( NativePtr() );
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

		// Will return a pointer to this resource if it is the same type.
		template<typename T> requires Concepts::IsRHIResource<T>
		T* As()
		{
			if ( Is<T>() )
			{
				return static_cast<T*>( this );
			}

			return nullptr;
		}

		// Will return a pointer to this resource if it is the same type.
		template<typename T> requires Concepts::IsRHIResource<T>
		const T* As() const
		{
			if ( Is<T>() )
			{
				return static_cast<const T*>( this );
			}
			return nullptr;
		}

		// Creates a handle to the existing RHI resource.
		template<Concepts::IsRHIResource T, typename... _Args>
		static T::RefType CreateHandle( T* a_Resource )
		{
			static constexpr auto deleter = +[]( T* a_Resource ) { delete a_Resource; };
			return T::RefType( a_Resource, deleter );
		}

		RHIResourceRef SharedFromThis()
		{
			return shared_from_this();
		}

		RHIResourceWeakRef WeakFromThis()
		{
			return weak_from_this();
		}

	protected:
		RHIResource() = default;
		virtual ~RHIResource() = default;
	};



	//=======================================================
	// RHI Resource Descriptor
	//  A base class for describing an RHI resource.
	//=======================================================
	template<typename _Descriptor, typename _Resource>
	struct RHIResourceDescriptor
	{
		using Super = RHIResourceDescriptor<_Descriptor, _Resource>;
		using ResourceType = _Resource;
		StringView Name;

		constexpr _Descriptor& SetName( StringView a_Name )
		{
			Name = a_Name;
			return static_cast<_Descriptor&>( *this );
		}
	};

	//==========================================================
	// RHI Resource barrier
	//  Describes a state transition for an RHI resource.
	//  Can be used for manual state transitions for RHI resources via RHI::TransitionResourceStates.
	//==========================================================
	struct RHIResourceBarrier
	{
		RHIResource* Resource = nullptr;
		ERHIResourceStates Before = ERHIResourceStates::Unknown;
		ERHIResourceStates After = ERHIResourceStates::Unknown;
	};

} // namespace Tridium

// Helper macro for defining a base RHI resource type, such as RHITexture, RHISampler, etc.
// _ClassName: The name of the resource type. Should be prefixed with RHI. E.g. _ClassName = RHITexture
// Note: RHI_RESOURCE_INTERFACE_BODY must be used with this macro.
#define DECLARE_RHI_RESOURCE_INTERFACE( _ClassName ) \
	class _ClassName; \
	struct _ClassName##Descriptor; \
	using _ClassName##Ref = ::Tridium::SharedPtr<_ClassName>; \
	using _ClassName##WeakRef = ::Tridium::WeakPtr<_ClassName>; \
	class _ClassName : public RHIResource

// Helper macro for defining the body of a base RHI resource type.
// _ClassName: The name of the resource type. Should be prefixed with RHI. E.g. _ClassName = RHITexture
// _RHIResourceType: The type of the resource. E.g. _RHIResourceType = ERHIResourceType::Texture
// Note: Must be used in the body of a DECLARE_RHI_RESOURCE_INTERFACE declaration.
#define RHI_RESOURCE_INTERFACE_BODY( _ClassName, _RHIResourceType ) \
public: \
	using DescriptorType = _ClassName##Descriptor; \
	using RefType = _ClassName##Ref; \
	using WeakRefType = _ClassName##WeakRef; \
	static constexpr ::Tridium::ERHIResourceType Type = ::Tridium::_RHIResourceType; \
	::Tridium::ERHIResourceType GetType() const override { return Type; } \
	const DescriptorType& Descriptor() const { return m_Desc; } \
	RefType SharedFromThis() { return std::static_pointer_cast<_ClassName>( std::move( shared_from_this() ) ); } \
	WeakRefType WeakFromThis() { return std::static_pointer_cast<_ClassName>( std::move( shared_from_this() ) ); } \
protected: \
	DescriptorType m_Desc; \
public:


// Helper macro for defining an RHI resource descriptor.
// _ClassName: The name of the resource descriptor. Should be prefixed with RHI and the suffix should be Descriptor. E.g. _ClassName = RHITextureDescriptor
// _Resource: The resource type that the descriptor describes. E.g. _Resource = RHITexture
#define DECLARE_RHI_RESOURCE_DESCRIPTOR( _ClassName, _Resource ) \
	struct _ClassName : public ::Tridium::RHIResourceDescriptor<_ClassName, class _Resource>

// Helper macro for defining a graphics API specific implementation of an RHI resource.
// _ClassName: The name of the resource implementation. Should be prefixed with the graphics API. E.g. _ClassName = RHITexture_OpenGLImpl
// _ParentResource: The parent resource type that the implementation inherits from. E.g. _ParentResource = RHITexture
// Note: RHI_RESOURCE_IMPLEMENTATION_BODY must be used with this macro.
#define DECLARE_RHI_RESOURCE_IMPLEMENTATION( _ClassName, _ParentResource ) \
	class _ClassName; \
	struct _ClassName##Descriptor; \
	class _ClassName : public _ParentResource

// Helper macro for defining the body of a graphics API specific implementation of an RHI resource.
// _ClassName: The name of the resource implementation. Should be prefixed with the graphics API. E.g. _ClassName = RHITexture_OpenGLImpl
// _RHIInterfaceType: The type of the RHI interface. E.g. _RHIInterfaceType = ERHInterfaceType::OpenGL
// Note: Must be used in the body of a DECLARE_RHI_RESOURCE_IMPLEMENTATION declaration.
#define RHI_RESOURCE_IMPLEMENTATION_BODY( _ClassName, _RHIInterfaceType ) \
public: \
	static constexpr ::Tridium::ERHInterfaceType API = _RHIInterfaceType;