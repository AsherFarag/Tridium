#pragma once
#include "RHICommon.h"

namespace Tridium {

	// = Forward Declarations =
	class IDynamicRHI;

	class IRHIObject;
	class IRHIResource;
	class IRHITexture;
	class IRHIShaderModule;
	class IRHIBuffer;
	class IRHIBindingLayout;
	class IRHIBindingSet;
	class IRHIGraphicsPipelineState;
	class IRHIComputePipelineState;
	class IRHICommandList;
	class IRHISwapChain;

	struct RHIObjectDesc;
	struct RHIResourceDesc;
	struct RHITextureDesc;
	struct RHIShaderModuleDesc;
	struct RHIBufferDesc;
	struct RHIBindingLayoutDesc;
	struct RHIBindingSetDesc;
	struct RHIGraphicsPipelineStateDesc;
	struct RHIComputePipelineStateDesc;
	struct RHICommandListDesc;
	struct RHISwapChainDesc;

	using RHIObjectRef                    = SharedPtr<IRHIObject>;
	using RHIResourceRef                  = SharedPtr<IRHIResource>;
	using RHITextureRef                   = SharedPtr<IRHITexture>;
	using RHIShaderModuleRef              = SharedPtr<IRHIShaderModule>;
	using RHIBufferRef		              = SharedPtr<IRHIBuffer>;
	using RHIBindingLayoutRef             = SharedPtr<IRHIBindingLayout>;
	using RHIBindingSetRef                = SharedPtr<IRHIBindingSet>;
	using RHIGraphicsPipelineStateRef     = SharedPtr<IRHIGraphicsPipelineState>;
	using RHIComputePipelineStateRef      = SharedPtr<IRHIComputePipelineState>;
	using RHICommandListRef               = SharedPtr<IRHICommandList>;
	using RHISwapChainRef                 = SharedPtr<IRHISwapChain>;

	using RHIObjectWeakRef                = WeakPtr<IRHIObject>;
	using RHIResourceWeakRef              = WeakPtr<IRHIResource>;                 
	using RHITextureWeakRef               = WeakPtr<IRHITexture>;
	using RHIShaderModuleWeakRef          = WeakPtr<IRHIShaderModule>;
	using RHIBufferWeakRef	              = WeakPtr<IRHIBuffer>;
	using RHIBindingLayoutWeakRef         = WeakPtr<IRHIBindingLayout>;
	using RHIBindingSetWeakRef            = WeakPtr<IRHIBindingSet>;
	using RHIGraphicsPipelineStateWeakRef = WeakPtr<IRHIGraphicsPipelineState>;
	using RHIComputePipelineStateWeakRef  = WeakPtr<IRHIComputePipelineState>;
	using RHICommandListWeakRef           = WeakPtr<IRHICommandList>;
	using RHISwapChainWeakRef             = WeakPtr<IRHISwapChain>;
	// ===================

	namespace Concepts {

		template<typename T>
		concept IsRHIResourceImplementation = Derived<T, IRHIObject>&& requires (T) { { T::API } -> std::convertible_to<ERHInterfaceType>; };

	} // namespace Concepts

	//======================================================================================================
	// RHI Object Interface
	//  An interface class that represents a child device object.
	//  Resources can be textures, buffers, samplers, etc.
	//  For graphics APIs, a specific implementation of this class will be created.
	//  E.g. RHICommandList_OpenGLImpl -> IRHICommandList -> IRHIObject
	//		 RHITexture_OpenGLImpl -> IRHITexture -> IRHIResource -> IRHIObject
	class IRHIObject : public EnableSharedFromThis<IRHIObject>
	{
    public:
        NON_COPYABLE_OR_MOVABLE( IRHIObject );

		// Creates a new RHI object of the specified type and forwards the 'a_Args' to its constructor.
		template<Concepts::Derived<IRHIObject> T, typename... _Args>
		static T::RefType Create( IDynamicRHI* a_RHI, _Args&&... a_Args )
		{
			return MakeShared<EnableMakeShared<T>>( a_RHI, std::forward<_Args>( a_Args )... );
		}

		// Returns the RHI device that owns this object.
		IDynamicRHI* Device() const { return m_Device; }

		// Releases the this device object, freeing it from the parent device.
		virtual bool Release() = 0;

		// Returns the type of the resource.
		virtual ERHIObjectType Type() const = 0;

		// Returns whether this resource is in a usable state.
		virtual bool Valid() const = 0;

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
		template<Concepts::Derived<IRHIObject> T>
		bool Is() const
		{
			if constexpr ( Concepts::IsRHIResourceImplementation<T> )
			{
				return Type() == T::StaticType()
					&& RHI::GetRHIType() == T::API;
			}

			return Type() == T::StaticType();
		}

		// Checked cast to the specified type in Debug mode, otherwise a static cast.
		template<Concepts::Derived<IRHIObject> T>
		T* As()
		{
		#if RHI_DEBUG_ENABLED
			if ( !Is<T>() )
			{
				return nullptr;
			}
		#endif

			return Cast<T*>(this);
		}

		// Checked cast to the specified type in Debug mode, otherwise a static cast.
		template<Concepts::Derived<IRHIObject> T>
		const T* As() const
		{
		#if RHI_DEBUG_ENABLED
			if ( !Is<T>() )
			{
				return nullptr;
			}
		#endif

			return Cast<const T*>( this );
		}

	protected:
		IRHIObject( IDynamicRHI* a_Device ) : m_Device( a_Device ) { RHI_DEV_CHECK( a_Device != nullptr, "RHI device cannot be null!" ); }
		virtual ~IRHIObject();

	private:
		IDynamicRHI* m_Device = nullptr; // Pointer to the RHI device that owns this object.
	};

	//==========================================================
	// RHI Resource Interface
	//  An interface class that represents a GPU resource (texture or a buffer).
	class IRHIResource : public IRHIObject
	{
	public:
		// Returns the internal state of the buffer.
		ERHIResourceStates State() const { return m_State; }

		// Sets the internal state of the buffer.
		// NOTE: This does not perform a state transition. This only sets the internal state of the buffer.
		//       Should only be used if manual state transitions have been completed and you want to return state management to the RHI.
		void SetState( ERHIResourceStates a_State ) { m_State = a_State; }

	protected:
		ERHIResourceStates m_State = ERHIResourceStates::Unknown;

		IRHIResource( IDynamicRHI* a_Device ) : IRHIObject( a_Device ) {}
		virtual ~IRHIResource() = default;
	};

	//==========================================================
	// RHI Resource barrier
	//  Describes a state transition for an RHI resource.
	//  Can be used for manual state transitions for RHI resources via RHI::TransitionResourceStates.
	struct RHIResourceBarrier
	{
		IRHIResource* Resource = nullptr;
		ERHIResourceStates Before = ERHIResourceStates::Unknown;
		ERHIResourceStates After = ERHIResourceStates::Unknown;
	};

} // namespace Tridium

#define DECLARE_RHI_OBJECT_IMPLEMENTATION( _C, P) class _C : public P

// Helper macro for defining the body of a base RHI resource type.
// _RHIResourceType: The type of the resource. E.g. _RHIResourceType = Texture.
//		This is used for string concatenation in the macro to define the type of the resource and its descriptor class.
// Note: Must be used in the body of a DECLARE_RHI_RESOURCE_INTERFACE declaration.
#define RHI_OBJECT_INTERFACE_BODY( _RHIObjectType ) \
public: \
	using DescriptorType = RHI##_RHIObjectType##Desc; \
	using RefType = RHI##_RHIObjectType##Ref; \
	using WeakRefType = RHI##_RHIObjectType##WeakRef; \
	static constexpr ::Tridium::ERHIObjectType StaticType() { return ::Tridium::ERHIObjectType::_RHIObjectType; } \
	::Tridium::ERHIObjectType Type() const override { return StaticType(); } \
	const DescriptorType& Desc() const { return m_Desc; } \
	RefType SharedFromThis() { return std::static_pointer_cast<IRHI##_RHIObjectType>( std::move( shared_from_this() ) ); } \
	WeakRefType WeakFromThis() { return std::static_pointer_cast<IRHI##_RHIObjectType>( std::move( shared_from_this() ) ); } \
protected: \
	DescriptorType m_Desc; \
public:

// Helper macro for defining the body of a graphics API specific implementation of an RHIObject.
// _ClassName: The name of the resource implementation. Should be prefixed with the graphics API. E.g. _ClassName = RHITexture_OpenGLImpl
// _RHIInterfaceType: The type of the RHI interface. E.g. _RHIInterfaceType = ERHInterfaceType::OpenGL
#define RHI_OBJECT_IMPLEMENTATION_BODY( _ClassName, _GraphicsAPI, _RHIInterfaceType ) \
public: \
	static constexpr ::Tridium::ERHInterfaceType API = _RHIInterfaceType; \
	using DeviceType = class DynamicRHI_##_GraphicsAPI##Impl; \
	DeviceType* Device() const { return reinterpret_cast<DeviceType*>( IRHIObject::Device() ); }