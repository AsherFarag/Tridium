#pragma once
#include <Tridium/Utils/Log.h>
#include <Tridium/Containers/Deque.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/RHI/RHICommon.h>
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Graphics/RHI/RHIVertexLayout.h>
#include <Tridium/Graphics/RHI/RHISampler.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHIShader.h>
#include <Tridium/Graphics/RHI/RHIShaderBindings.h>
#include <Tridium/Graphics/RHI/RHISwapChain.h>
#include <Tridium/Graphics/RHI/RHIDescriptorAllocator.h>
#include <Tridium/Graphics/RHI/RHIStateTracker.h>

#include "D3D12.h"
#include "D3D12ToRHI.h"
#include "ThirdParty/D3D12MemAlloc.h"

DECLARE_LOG_CATEGORY( DirectX );

#if RHI_USE_DEBUG_NAMES
	#define D3D12_SET_DEBUG_NAME( _Object, _Name, _DefaultName ) \
	do { \
		if ( RHI::IsDebug() && static_cast<bool>( _Object ) ) \
		{ \
			::Tridium::WString wName = !_Name.empty() ? WString( _Name.begin(), _Name.end() ) : WString( _DefaultName ); \
			_Object->SetName( wName.c_str() ); \
			::Tridium::D3D12::D3D12Context::Get()->StringStorage.EmplaceBack( std::move( wName ) ); \
		} \
	} while ( false )
#else
#define D3D12_SET_DEBUG_NAME( _Object, _Name, _DefaultName ) do {} while ( false )
#endif // RHI_USE_DEBUG_NAMES

namespace Tridium::D3D12 {

	//================================
	// Forward declarations
	class RHITexture_D3D12Impl;
	class RHIBuffer_D3D12Impl;
	class RHISampler_D3D12Impl;
	class RHIShaderModule_D3D12Impl;
	class RHISwapChain_D3D12Impl;
	class RHIGraphicsPipelineState_D3D12Impl;
	class RHICommandList_D3D12Impl;
	class DynamicRHI_D3D12Impl;
	//=================================


	using DescriptorIndex = uint32_t;
	using RootParameterIndex = uint32_t;
	constexpr RootParameterIndex c_InvalidRootParameterIndex = ~0u;

	template<typename T>
	inline constexpr T AlignUp( T a_Size, T a_Alignment )
	{
		return (a_Size + a_Alignment - 1 ) & ~( a_Alignment - 1 );
	}

	inline D3D12_RESOURCE_BARRIER Translate( RHIResourceBarrier a_Barrier )
	{
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = a_Barrier.Resource->NativePtrAs<ID3D12Resource>();
		barrier.Transition.StateBefore = Translate( a_Barrier.Before );
		barrier.Transition.StateAfter = Translate( a_Barrier.After );
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		return barrier;
	}

	//=====================================================================
	// Descriptor Heap Flags
	//  Bitmask flags describing the properties of a descriptor heap.
	enum class EDescriptorHeapFlags : uint8_t
	{
		None = 0,
		// The heap is visible to the GPU.
		GPUVisible = 1 << 0,
		// The heap is poolable.
		Poolable = 1 << 1,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( EDescriptorHeapFlags );

	inline D3D12_DESCRIPTOR_HEAP_FLAGS Translate( EDescriptorHeapFlags a_Flags )
	{
		return EnumFlags( a_Flags ).HasFlag( EDescriptorHeapFlags::GPUVisible ) 
			? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	}

	struct RootSignature
	{
		hash64_t Hash = 0;
		InlineArray<Pair<RHIBindingLayoutRef, RootParameterIndex>, RHIConstants::MaxBindingLayouts> Layouts;
		ComPtr<ID3D12RootSignature> D3D12Signature;
		uint32_t InlinedConstantsSize = 0; // Size of the inlined constants in bytes
		RootParameterIndex RootParamInlinedConstants = c_InvalidRootParameterIndex;

		bool Valid() const { return D3D12Signature && Layouts.Size(); }
		static RootSignature Build( Span<const RHIBindingLayoutRef> a_Layouts, bool a_AllowInputLayout, bool a_IsLocal, Span<const D3D12_ROOT_PARAMETER1> a_CustomParams = {} );
	};

	struct D3D12TierInfo
	{
		size_t MaxShaderVisibleDescriptorHeapSize = 0;
		size_t MaxShaderVisibleSamplerHeapSize = 0;
	};

	inline const D3D12TierInfo& TierInfo() { return { D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2, D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE }; }

	// Returns the Number
	template<typename T> requires std::is_base_of_v<IUnknown, T>
	inline ULONG ForceDeleteIUnknown( T** const a_Object )
	{
		if ( !a_Object || !(*a_Object) ) return 0;
		ULONG refCount = (*a_Object)->Release();
		// Keep calling Release until the reference count is 0
		if ( refCount )
			while ( (*a_Object)->Release() ) {}
		(*a_Object) = nullptr;
		return refCount;
	}

	//======================================================================
	// Device Child
	//  A base class for an object that is owned by a D3D12 device.
	class DeviceChild
	{
	public:
		NON_COPYABLE( DeviceChild );
		DeviceChild() = delete;

		DeviceChild( ID3D12Device* a_ParentDevice )
			: m_ParentDevice( a_ParentDevice )
		{}

		DeviceChild( DeviceChild&& other ) noexcept
			: m_ParentDevice( std::exchange( other.m_ParentDevice, nullptr ) ) 
		{}

		DeviceChild& operator=( DeviceChild&& other ) noexcept
		{
			if ( this != &other )
			{
				m_ParentDevice = std::exchange( other.m_ParentDevice, nullptr );
			}
			return *this;
		}

		virtual ~DeviceChild() = default;

		ID3D12Device* ParentDevice() const { return m_ParentDevice; }

	private:
		ID3D12Device* m_ParentDevice;
	};

	// Common Heap Properties
	constexpr struct {

		const D3D12_HEAP_PROPERTIES Default = {
			.Type = D3D12_HEAP_TYPE_DEFAULT,
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0u, // 0 means all nodes
			.VisibleNodeMask = 0u  // 0 means all nodes
		};

		const D3D12_HEAP_PROPERTIES Upload = {
			.Type = D3D12_HEAP_TYPE_UPLOAD,
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0u, // 0 means all nodes
			.VisibleNodeMask = 0u  // 0 means all nodes
		};

		const D3D12_HEAP_PROPERTIES Readback = {
			.Type = D3D12_HEAP_TYPE_READBACK,
			.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
			.CreationNodeMask = 0u, // 0 means all nodes
			.VisibleNodeMask = 0u  // 0 means all nodes
		};

	} HeapProperties;

	//======================================================================
	// Managed Resource
	//  A wrapper around a D3D12 resource and its allocation using D3D12MA.
	struct ManagedResource
	{
		ComPtr<D3D12MA::Allocation> Allocation{};

		~ManagedResource()
		{
			Release();
		}

		bool Valid() const { return Allocation; }
		
		ID3D12Resource* Resource() const { return Valid() ? Allocation->GetResource() : nullptr; }
		ID3D12Resource** ResourceAddress() { return Valid() ? Allocation->GetResourceAddress() : nullptr; }

		void Release()
		{
			if ( Valid() )
			{
				Allocation.Reset();
			}
		}

		bool Commit( const D3D12_RESOURCE_DESC& a_ResourceDesc,
			D3D12_RESOURCE_STATES a_InitialState,
			const D3D12_CLEAR_VALUE* a_ClearValue = nullptr,
			D3D12MA::Allocator* a_Allocator = nullptr )
		{
			D3D12MA::ALLOCATION_DESC allocDesc{};
			allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
			return Commit( a_ResourceDesc, allocDesc, a_InitialState, a_ClearValue, a_Allocator );
		}

		bool Commit( const D3D12_RESOURCE_DESC& a_ResourceDesc,
			const D3D12MA::ALLOCATION_DESC& a_AllocDesc,
			D3D12_RESOURCE_STATES a_InitialState,
			const D3D12_CLEAR_VALUE* a_ClearValue = nullptr,
			D3D12MA::Allocator* a_Allocator = nullptr );
	};

	//======================================================================
	// Command Context
	//  Represents a command list instance. 
	//  Contains strong references to all the resources that are used by the command list.
	struct CommandContext
	{
		ERHICommandQueueType QueueType = ERHICommandQueueType::Graphics;
		ComPtr<ID3D12CommandAllocator> CmdAllocator = nullptr;
		ComPtr<ID3D12CommandList> CmdList = nullptr;
		ComPtr<ID3D12Fence> Fence = nullptr;
		RHIFenceValue SubmittedValue = 0;

		Array<RHIObjectRef> ReferencedResources{};
		Array<SharedPtr<class DescriptorHeap>> DescriptorHeaps{};
		Array<ComPtr<IUnknown>> ReferencedUnknowns{};

		~CommandContext()
		{
			ReferencedResources.Clear();
			DescriptorHeaps.Clear();
			ReferencedUnknowns.Clear();
			CmdList.Reset();
			CmdAllocator.Reset();
			Fence.Reset();
		}
	};

	//======================================================================
	// Command Queue
	//  A wrapper around a D3D12 command queue and its associated fence.
	//  In a Dynamic RHI context, 3 Command Queues are created:
	//  - Graphics Command Queue
	//  - Compute Command Queue
	//  - Copy Command Queue
	//  Each command queue has its own fence to track the completion of submitted commands.
	struct CommandQueue
	{
		ComPtr<ID3D12CommandQueue> CmdQueue = nullptr;
		ComPtr<ID3D12Fence> Fence = nullptr;
		RHIFenceValue LastSubmittedValue = 0;
		RHIFenceValue LastCompletedValue = 0;
		Deque<CommandContext> CmdContextsInFlight{};

		RHIFenceValue Signal()
		{
			CmdQueue->Signal( Fence.Get(), ++LastSubmittedValue );
			return LastSubmittedValue;
		}

		RHIFenceValue UpdateLastCompletedValue()
		{
			if ( LastCompletedValue < LastSubmittedValue )
				LastCompletedValue = Fence->GetCompletedValue();
			return LastCompletedValue;
		}

		void WaitForFence( HANDLE a_FenceEvent, RHIFenceValue a_FenceValue )
		{
			if ( UpdateLastCompletedValue() < a_FenceValue )
			{
				ResetEvent( a_FenceEvent );
				Fence->SetEventOnCompletion( a_FenceValue, a_FenceEvent );
				WaitForSingleObject( a_FenceEvent, INFINITE );
			}
		}

		CommandQueue( ID3D12Device& a_D3D12Device, ID3D12CommandQueue* a_D3D12CmdQueue )
			: CmdQueue( a_D3D12CmdQueue )
		{
			ENSURE( a_D3D12CmdQueue, "Command queue cannot be null" );
			a_D3D12Device.CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( Fence.GetAddressOf() ) );
		}

		~CommandQueue()
		{
			CmdContextsInFlight.clear();
			CmdQueue.Reset();
			Fence.Reset();
		}
	};

	//======================================================================
	// Upload Buffer
	//  A ring buffer for uploading data to the GPU.
	class UploadBuffer
	{
	public:
		bool Commit( size_t a_BufferSize, D3D12MA::Allocator& a_Allocator )
		{
			m_BufferSize = a_BufferSize;

			D3D12_RESOURCE_DESC bufferDesc = {};
			bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Width = m_BufferSize;
			bufferDesc.Height = 1;
			bufferDesc.DepthOrArraySize = 1;
			bufferDesc.MipLevels = 1;
			bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12MA::ALLOCATION_DESC allocDesc = {};
			allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			return m_UploadBuffer.Commit( bufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &a_Allocator );
		}

		void Release()
		{
			m_UploadBuffer.Release();
			m_BufferSize = 0;
			m_CurrentOffset = 0;
		}

		void* Allocate( size_t a_Size, size_t& o_Offset )
		{
			std::lock_guard lock( m_Mutex );

			// Align size to 256 bytes
			a_Size = (a_Size + 255) & ~255;

			if ( m_CurrentOffset + a_Size > m_BufferSize )
			{
				// Wrap around ( circular allocation )
				Reset();
			}

			o_Offset = m_CurrentOffset;
			m_CurrentOffset += a_Size;

			void* mappedData = nullptr;
			D3D12_RANGE range = { 0, 0 };
			m_UploadBuffer.Resource()->Map(0, &range, &mappedData);

			return ReinterpretCast<uint8_t*>(mappedData) + o_Offset;
		}

		void Reset()
		{
			m_CurrentOffset = 0;
		}

		ID3D12Resource* GetResource() const { return m_UploadBuffer.Resource(); }

	private:
		ManagedResource m_UploadBuffer{};
		size_t m_BufferSize = 0;
		size_t m_CurrentOffset = 0;
		std::mutex m_Mutex{};
	};

	//=====================================================================
	// Descriptor Heap
	//  Wrapper around a D3D12 descriptor heap.
	class DescriptorHeap : public DeviceChild
	{
	public:
		DescriptorHeap() = delete;
		// Create a new descriptor heap with its own D3D12 descriptor heap.
		DescriptorHeap( ID3D12Device* a_Device, ComPtr<ID3D12DescriptorHeap>&& a_Heap,
			uint32_t a_NumDescriptors, ERHIDescriptorHeapType a_Type,
			EDescriptorHeapFlags a_Flags, bool a_IsGlobal = false );
		// Create a new descriptor heap that is a suballocation of a larger heap.
		DescriptorHeap( DescriptorHeap& a_ParentHeap, uint32_t a_Offset, uint32_t a_NumDescriptors );
		~DescriptorHeap();

		ID3D12DescriptorHeap* Heap() const { return m_Heap.Get(); }
		ERHIDescriptorHeapType Type() const { return m_Type; }
		EDescriptorHeapFlags Flags() const { return m_Flags; }

		uint32_t Offset() const { return m_Offet; }
		uint32_t NumDescriptors() const { return m_NumDescriptors; }
		uint32_t DescriptorSize() const { return m_DescriptorSize; }
		bool IsGlobal() const { return m_IsGlobal; }
		bool IsSuballocation() const { return m_IsSuballocation; }
		bool IsPoolable() const { return m_Flags.HasFlag( EDescriptorHeapFlags::Poolable ); }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle( uint32_t a_Index ) const { return { m_CPUBase.ptr + a_Index * m_DescriptorSize }; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle( uint32_t a_Index ) const { return { m_GPUBase.ptr + a_Index * m_DescriptorSize }; }

		// Get the total size of the heap in bytes.
		size_t SizeInBytes() const { return m_NumDescriptors * m_DescriptorSize; }

	private:
		ComPtr<ID3D12DescriptorHeap> m_Heap;
		const ERHIDescriptorHeapType m_Type;
		const EnumFlags<EDescriptorHeapFlags> m_Flags;

		const D3D12_CPU_DESCRIPTOR_HANDLE m_CPUBase;
		const D3D12_GPU_DESCRIPTOR_HANDLE m_GPUBase;

		// True this heap is a global heap.
		const bool m_IsGlobal = false;

		// True if this heap is a suballocation of a larger heap.
		const bool m_IsSuballocation = false;
		// The offset of the first descriptor in the heap. Only used when the heap is a Suballocation.
		const uint32_t m_Offet = 0u;

		// The size of each descriptor in the heap.
		const uint32_t m_DescriptorSize;
		// The total number of descriptors in the heap.
		const uint32_t m_NumDescriptors;
	};
	using DescriptorHeapRef = SharedPtr<DescriptorHeap>;

	//=====================================================================
	// Descriptor Manager
	//  Implementation of RHIHeapDescriptorAllocator.
	class DescriptorManager : public RHIHeapDescriptorAllocator, public DeviceChild
	{
	public:
		DescriptorManager() = delete;
		DescriptorManager( ID3D12Device* a_Device, DescriptorHeapRef&& a_Heap )
			: RHIHeapDescriptorAllocator( a_Heap->Type(), a_Heap->NumDescriptors() )
			,DeviceChild( a_Device )
			, m_Heap( std::move( a_Heap ) ) 
		{}
		DescriptorManager( DescriptorManager&& other ) noexcept = default;
		DescriptorManager& operator=( DescriptorManager&& other ) noexcept = default;
		~DescriptorManager() = default;

		DescriptorHeap* Heap() { return m_Heap.get(); }
		const DescriptorHeap* Heap() const { return m_Heap.get(); }

		bool HandlesAllocation( ERHIDescriptorHeapType a_Type, EDescriptorHeapFlags a_Flags ) const
		{
			return RHIHeapDescriptorAllocator::HandlesAllocation( a_Type ) && m_Heap->Flags() == a_Flags;
		}

		// Returns true if the given heap is a child of this manager.
		bool IsHeapAChild( const DescriptorHeap& a_Heap ) const { return m_Heap->Heap() == a_Heap.Heap(); }

		void Release()
		{
			ENSURE( m_Heap.use_count() == 1, "Heap is still in use! " );
			m_Heap.reset();
		}

	private:
		DescriptorHeapRef m_Heap;
	};

	struct DescriptorHeapManagerDesc
	{
		uint32_t NumGlobalResourceDescriptors     = 16384; // Number of resource descriptors in their static heap. (SRV, UAV, CBV).
		uint32_t NumGlobalSamplerDescriptors      = 1024;  // Number of sampler descriptors in their static heap.
		uint32_t NumGlobalRenderTargetDescriptors = 1024;  // Number of render target descriptors in their static heap.
		uint32_t NumGlobalDepthStencilDescriptors = 1024;  // Number of depth stencil descriptors in their static heap.
	};

	//=====================================================================
	// Descriptor Heap Manager
	//  The primary descriptor heap manager for the device.
	class DescriptorHeapManager final
	{
	public:
		// Allocate a descriptor heap.
		DescriptorHeapRef AllocateIndependentHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, EDescriptorHeapFlags a_Flags, StringView a_DebugName = StringView{} );
		// Allocate a descriptor heap from the global heap pool.
		DescriptorHeapRef AllocateHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, EDescriptorHeapFlags a_Flags, StringView a_DebugName = StringView{} );

		// Frees the given heap immediately.
		void ImmediateFreeHeap( DescriptorHeapRef&& a_Heap );

		// Add a heap to the pool so that it can be reused.
		void AddHeapToPool( ComPtr<ID3D12DescriptorHeap>&& a_Heap, ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, EDescriptorHeapFlags a_Flags );

	private:
		bool m_Shutdown = false;
		ID3D12Device* m_Device = nullptr;
		Array<DescriptorManager> m_GlobalHeaps{};

		// A heap that is pooled for reuse.
		// Useful for descriptor heaps that are frequently created and destroyed.
		struct PooledHeap
		{
			ComPtr<ID3D12DescriptorHeap> Heap;
			uint32_t NumDescriptors;
			ERHIDescriptorHeapType Type;
			EDescriptorHeapFlags Flags;

			PooledHeap( ComPtr<ID3D12DescriptorHeap>&& a_Heap, ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, EDescriptorHeapFlags a_Flags )
				: Heap( std::move( a_Heap ) ), NumDescriptors( a_NumDescriptors ), Type( a_Type ), Flags( a_Flags ) {}
		};

		Array<PooledHeap> m_PooledHeaps{};
		std::mutex m_PooledHeapsMutex{};

	private:
		friend class DynamicRHI_D3D12Impl;

		NON_COPYABLE( DescriptorHeapManager );
		DescriptorHeapManager() = default;
		~DescriptorHeapManager() { ENSURE( m_Shutdown, "DescriptorHeapManager was not shutdown!" ); }

		// Initialize the descriptor heap manager
		void Init( ID3D12Device* a_Device, const DescriptorHeapManagerDesc& a_Desc );
		void Shutdown();

		// Try to retieve a pooled heap, can return null
		ComPtr<ID3D12DescriptorHeap> AcquirePooledHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, EDescriptorHeapFlags a_Flags );
	};

	//=======================================================
	// D3D12 Context
	//  A context for global D3D12 data.
	class D3D12Context : public ISingleton<D3D12Context, /* _ExplicitSetup */ false>
	{
	public:
		// A global storage for Wide Strings
		// Work around for owning strings as Tridium uses Strings instead of Wide Strings
		Array<WString> StringStorage;
	};

#pragma region D3D12 RHI IMPLEMENTATIONS

	//======================================================================
	// TEXTURE IMPLEMENTATION
	//=======================================================================

	class RHITexture_D3D12Impl : public IRHITexture
	{
		RHI_OBJECT_IMPLEMENTATION_BODY( RHITexture_D3D12Impl, D3D12, ERHInterfaceType::DirectX12 );

		RHITexture_D3D12Impl( IDynamicRHI* a_Device, const RHITextureDesc & a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData = {} );
		~RHITexture_D3D12Impl() override { Release(); }

		virtual bool Release() override;
		virtual const void* NativePtr() const override { return Texture.Resource(); }
		virtual bool Valid() const override { return Texture.Valid(); }

		// D3D12 specific functions
		[[nodiscard]] D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const;
		[[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC CreateSRVDesc( ERHIFormat a_Format, ERHITextureDimension a_Dimension, RHITextureSubresourceSet a_Subresources );

		ManagedResource Texture{};
	};

	//======================================================================
	// BUFFER IMPLEMENTATION
	//=======================================================================

	class RHIBuffer_D3D12Impl : public IRHIBuffer
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIBuffer_D3D12Impl, D3D12, ERHInterfaceType::DirectX12 );

		RHIBuffer_D3D12Impl( IDynamicRHI* a_Device, const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data = {} );
		~RHIBuffer_D3D12Impl() override { Release(); }

		virtual bool Release() override { ManagedBuffer.Release(); return true; }
		virtual bool Valid() const override { return ManagedBuffer.Valid(); }
		virtual const void* NativePtr() const override { return ManagedBuffer.Resource(); }

		// D3D12 specific functions
		D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const;
		D3D12_SHADER_RESOURCE_VIEW_DESC CreateSRVDesc( ERHIBufferType a_Type, RHIBufferRange a_Range = RHIBufferRange::EntireBuffer(), ERHIFormat a_Format = ERHIFormat::Unknown) const;
		D3D12_CONSTANT_BUFFER_VIEW_DESC CreateCBVDesc( RHIBufferRange a_Range = RHIBufferRange::EntireBuffer() ) const;

		ManagedResource ManagedBuffer{};
	};

	//======================================================================
	// BINDING LAYOUT IMPLEMENTATION
	//=======================================================================

	class RHIBindingLayout_D3D12Impl : public IRHIBindingLayout
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIBindingLayout_D3D12Impl, D3D12, ERHInterfaceType::DirectX12 );
		RHIBindingLayout_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		virtual ~RHIBindingLayout_D3D12Impl() { Release(); }

		bool Release() override;
		bool Valid() const override;
		const void* NativePtr() const override;

		uint32_t InlinedConstantsSize = 0; // Size of the inlined constants in bytes
		RootParameterIndex RootParamInlinedConstants = c_InvalidRootParameterIndex;
		RootParameterIndex RootParamRenderResources = c_InvalidRootParameterIndex;
		RootParameterIndex RootParamSamplers = c_InvalidRootParameterIndex;
		uint32_t DescriptorTableSizeRenderResources = 0; // Size of the descriptor table for SRV, UAV, CBV.
		uint32_t DescriptorTableSizeSamplers = 0;
		Array<D3D12_DESCRIPTOR_RANGE1> DescriptorRangesRenderResources{}; // Descriptor ranges for SRV, UAV,  CBV.
		Array<D3D12_DESCRIPTOR_RANGE1> DescriptorRangesSamplers{};
		Array<RHIShaderBinding> RenderResourceBindingLayouts{}; // All SRV, UAV, and CBV's in the binding layout.
		InlineArray<D3D12_ROOT_PARAMETER1, 32> RootParams;
	};

	//======================================================================
	// BINDING SET IMPLEMENTATION
	//=======================================================================

	class RHIBindingSet_D3D12Impl : public IRHIBindingSet
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIBindingSet_D3D12Impl, D3D12, ERHInterfaceType::DirectX12 );
		RHIBindingSet_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		~RHIBindingSet_D3D12Impl() override { Release(); }

		bool Release() override;
		bool Valid() const override;
		const void* NativePtr() const override;

		DescriptorIndex DescriptorTableRenderResources = 0;
		DescriptorIndex DescriptorTableSamplers = 0;
		RootParameterIndex RootParamRenderResources = 0;
		RootParameterIndex RootParamSamplers = 0;

		Array<RHIObjectRef> Resources{};
		SharedPtr<DescriptorHeap> RenderResourceHeap{}; // Heap for SRV, UAV, CBV descriptors.
		SharedPtr<DescriptorHeap> SamplerHeap{}; // Heap for sampler descriptors.
	};

	//======================================================================
	// SHADER MODULE IMPLEMENTATION
	//======================================================================

	class RHIShaderModule_D3D12Impl : public IRHIShaderModule
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIShaderModule_D3D12Impl, D3D12, ERHInterfaceType::DirectX12 )
		RHIShaderModule_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		~RHIShaderModule_D3D12Impl() override { Release(); }

		bool Release() override;
		bool Valid() const override;
		const void* NativePtr() const override;

		Array<Byte> Bytecode;
	};

	//======================================================================
	// GRAPHICS PIPELINE STATE IMPLEMENTATION
	//======================================================================

	class RHIGraphicsPipelineState_D3D12Impl : public IRHIGraphicsPipelineState
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHIGraphicsPipelineState_D3D12Impl, D3D12, ERHInterfaceType::DirectX12 );
		RHIGraphicsPipelineState_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType& a_Desc, SharedPtr<RootSignature> a_RootSig );
		~RHIGraphicsPipelineState_D3D12Impl() override { Release(); }

		bool Release() override;
		bool Valid() const override { return PSO != nullptr && RootSig != nullptr; }
		const void* NativePtr() const override { return PSO.Get(); }

		SharedPtr<RootSignature> RootSig;
		ComPtr<ID3D12PipelineState> PSO;
		InlineArray<D3D12_INPUT_ELEMENT_DESC, RHIConstants::MaxVertexAttributes> VertexLayout;
	};

	//======================================================================
	// SWAPCHAIN IMPLEMENTATION
	//======================================================================

	class RHISwapChain_D3D12Impl : public IRHISwapChain
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHISwapChain_D3D12Impl, D3D12, ERHInterfaceType::DirectX12 );
		RHISwapChain_D3D12Impl( IDynamicRHI* a_Device, const DescriptorType & a_Desc );
		~RHISwapChain_D3D12Impl() override { Release(); }

		bool Release() override;
		bool Valid() const override;
		const void* NativePtr() const override { return SwapChain.Get(); }
		bool Present() override;
		RHITextureRef GetBackBuffer() override;
		bool Resize( uint32_t a_Width, uint32_t a_Height ) override;
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		ComPtr<IDXGISwapChain3> SwapChain;
		InlineArray<RHITextureRef, RHIConstants::MaxColorTargets> RTVs;

	private:
		RHIFenceValue m_LastPresentedValue = 0;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		bool m_ShouldResize = false;

		bool ResizeBuffers();
		void ReleaseBuffers();
		bool GetBackBuffers();
	};

	//======================================================================
	// COMMAND LIST IMPLEMENTATION
	//======================================================================

	class RHICommandList_D3D12Impl : public IRHICommandList
	{
	public:
		RHI_OBJECT_IMPLEMENTATION_BODY( RHICommandList_D3D12Impl, D3D12, ERHInterfaceType::DirectX12 );
		RHICommandList_D3D12Impl( IDynamicRHI* a_Device, const RHICommandListDesc& a_Desc );
		~RHICommandList_D3D12Impl() override { Release(); }

		bool Release() override;
		bool Valid() const override { return true; }
		const void* NativePtr() const override { return GetD3D12CmdList(); }
		bool IsImmediate() const override { return false; } // Immediate command lists are not supported in D3D12.

		bool Open() override;
		bool Close() override;
		void ClearState() override;

		void ResourceBarriers( Span<const RHIResourceBarrier> a_Barriers, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void UpdateBuffer( IRHIBuffer& a_Buffer, const void* a_Data, size_t a_DataSizeBytes, size_t a_DstOffsetBytes = 0, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void CopyBuffer( IRHIBuffer& a_DstBuffer, size_t a_DstOffsetBytes, IRHIBuffer& a_SrcBuffer, RHIBufferRange a_SrcRange, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void UpdateTexture( IRHITexture& a_Texture, const RHITextureSlice& a_DstSlice, RHITextureSubresourceData a_Data, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void CopyTexture( IRHITexture& a_DstTexture, const RHITextureSlice& a_DstSlice, IRHITexture& a_SrcTexture, const RHITextureSlice& a_SrcSlice, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void SetInlinedConstants( const void* a_Data, uint32_t a_SizeBytes, uint32_t a_DstOffsetBytes = 0, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void SetGraphicsState( const RHIGraphicsState& a_GraphicsState, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void ClearRenderTargets( ERHIClearFlags a_Flags, RHIClearValue a_ClearValue, int32_t a_ColorAttachmentIndex = -1, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void SetViewportState( const RHIViewportState& a_Viewports, RHI_DEBUG_SRC_LOC_PARAM ) override;
		void Draw( const RHIDrawArgs& a_DrawArgs, RHI_DEBUG_SRC_LOC_PARAM ) override;

		void PushDebugGroup( StringView a_Name ) override;
		void PopDebugGroup() override;
		void InsertDebugMarker( StringView a_Name ) override;

		// = D3D12 Specific =

		ID3D12GraphicsCommandList* GetD3D12CmdList() const { return m_ActiveCmdList.CmdList.Get(); }
		CommandContext ReleaseCmdContext( CommandQueue& a_CmdQueue );

	private:
		//======================================================================
		// Command List
		//  A simple wrapper around a D3D12 command list and its associated command allocator.
		//  A pool of these command lists are created per RHICommandList_D3D12Impl instance.
		struct CommandList
		{
			ComPtr<ID3D12CommandAllocator> CmdAllocator = nullptr;
			ComPtr<ID3D12GraphicsCommandList> CmdList = nullptr;
			RHIFenceValue LastSubmittedValue = 0;

			bool Valid() const { return CmdList != nullptr && CmdAllocator != nullptr; }
		};

		CommandQueue* m_CmdQueue = nullptr;
		RHIResourceStateTracker m_ResourceStateTracker{};
		Array<D3D12_RESOURCE_BARRIER> m_D3D12Barriers{};

		DescriptorHeap* m_RTVHeap = nullptr;
		DescriptorHeap* m_DSVHeap = nullptr;
		DescriptorHeap* m_SRVUAVHeap = nullptr;
		DescriptorHeap* m_SamplerHeap = nullptr;

		bool m_GraphicsStateValid = false; // Whether the graphics state has been set.
		RHIGraphicsState m_CurrentGraphicsState{}; // Current graphics state for the command list.

		Deque<CommandList> m_CmdListPool{};
		CommandList m_ActiveCmdList{}; // The currently active command list that is being recorded to.
		CommandContext m_CmdContext{}; // The current command context that is being used to record commands.

	private:
		void CommitBarriers();
		void BindGraphicsPipelineState( RHIGraphicsPipelineState_D3D12Impl* a_PSO, bool a_UpdateRootSignature );
		void BindFramebuffer( const RHIFramebuffer& a_Framebuffer );
		void BindGraphicsBindings( Span<IRHIBindingSet const* const> a_BindingSets, uint32_t a_UpdateMask, const SharedPtr<RootSignature>& a_RootSignature );

		// Helper for allocating and registering a descriptor heap to the command list.
		const SharedPtr<DescriptorHeap>& AllocateHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, EDescriptorHeapFlags a_Flags, StringView a_DebugName = {} );
	};

	//======================================================================
	// DYNAMIC RHI IMPLEMENTATION
	//======================================================================

	class DynamicRHI_D3D12Impl final : public IDynamicRHI
	{
	public:
		//==============================================
		// Core RHI functions
		bool Init( const RHIConfig& a_Config ) override;
		bool Shutdown() override;
		void BeginFrame() override;
		void EndFrame() override;
		RHIFenceValue ExecuteCommandLists( Span<IRHICommandList* const> a_CommandLists, ERHICommandQueueType a_QueueType ) override;
		bool WaitForIdle() override;
		void WaitForFence( ERHICommandQueueType a_QueueType, RHIFenceValue a_FenceValue ) override;
		void CollectGarbage() override;
		ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::DirectX12; }
		static constexpr ERHInterfaceType GetStaticRHIType() { return ERHInterfaceType::DirectX12; }
		//==============================================

		//=====================================================
		// Resource creation
		virtual RHITextureRef CreateTexture( const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData ) override;
		virtual RHIBufferRef CreateBuffer( const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data ) override;
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDesc& a_Desc ) override;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDesc& a_Desc ) override;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDesc& a_Desc ) override;
		virtual RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDesc& a_Desc ) override;
		virtual RHIBindingSetRef CreateBindingSet( const RHIBindingSetDesc& a_Desc ) override;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDesc& a_Desc ) override;
		//=====================================================

		//=====================================================
		// Miscellaneous
		IRHISwapChain* GetSwapChain() const override { return m_SwapChain.get(); }
		virtual GPUInfo GetGPUInfo() const override;
		//=====================================================

		//====================================================
		// D3D12 Specific
		//====================================================

		CommandQueue* GetCommandQueue( ERHICommandQueueType a_Type ) const
		{
			RHI_DEV_CHECK( a_Type < ERHICommandQueueType::COUNT, "Invalid command queue type!" );
			return m_CmdQueues.At( Cast<size_t>( a_Type ) ).get();
		}

		auto* GetResourceInitCommandList() { return m_ResourceInitCmdList->As<RHICommandList_D3D12Impl>(); }
		const auto& GetDXGIFactory() const { return m_DXGIFactory; }
		const auto& GetDXGIAdapter() const { return m_DXGIAdapter; }
		const auto& GetAllocator() const { return m_Allocator; }
		const auto& GetUploadBuffer() const { return m_UploadBuffer; }
		auto& GetUploadBuffer() { return m_UploadBuffer; }

		D3D12::DescriptorHeapManager& GetDescriptorHeapManager() { return m_DescriptorHeapManager; }
		const D3D12::DescriptorHeapManager& GetDescriptorHeapManager() const { return m_DescriptorHeapManager; }

		bool SupportsDeviceVersion( uint32_t a_Version ) const { return m_MaxD3D12DeviceVersion >= a_Version; }
		ID3D12Device* GetD3D12Device() const { return m_Device.Get(); }

	#define GET_D3D12_DEVICE( _Version ) \
		ID3D12Device##_Version* GetD3D12Device##_Version() const \
		{ \
			RHI_DEV_CHECK( m_MaxD3D12DeviceVersion >= _Version, "D3D12Device" #_Version " is not supported! Max supported version is: ", m_MaxD3D12DeviceVersion ); \
			return Cast<ID3D12Device##_Version*>( m_Device.Get() ); \
		}

		GET_D3D12_DEVICE( 1 );
		GET_D3D12_DEVICE( 2 );
		GET_D3D12_DEVICE( 3 );
		GET_D3D12_DEVICE( 4 );
		GET_D3D12_DEVICE( 5 );

	#undef GET_D3D12_DEVICE

		//====================================================

	private:
		ComPtr<ID3D12Device> m_Device = nullptr;
		ComPtr<IDXGIAdapter> m_DXGIAdapter = nullptr;
		ComPtr<IDXGIFactory> m_DXGIFactory = nullptr;
		ComPtr<D3D12MA::Allocator> m_Allocator = nullptr;
		DescriptorHeapManager m_DescriptorHeapManager{};
		RHISwapChainRef m_SwapChain = nullptr;

		uint32_t m_MaxD3D12DeviceVersion = 0;

		UnorderedMap<hash64_t, WeakPtr<RootSignature>> m_RootSignatureCache{};

		UploadBuffer m_UploadBuffer{};
		RHICommandListRef m_ResourceInitCmdList = nullptr;
		FixedArray<UniquePtr<CommandQueue>, size_t( ERHICommandQueueType::COUNT )> m_CmdQueues{};
		HANDLE m_FenceEvent = nullptr;
		Array<ID3D12CommandList*> m_CmdListsToExecute{};

		//=====================================================

	private:
		// Will either find an existing root signature or create a new one.
		SharedPtr<RootSignature> GetRootSignature( Span<const RHIBindingLayoutRef> a_BindingLayouts, bool a_AllowInputLayout );

	#if RHI_DEBUG_ENABLED
	public:
		virtual void DumpDebug() override;

	private:
		ComPtr<ID3D12Debug> m_D3D12Debug = nullptr;
		ComPtr<IDXGIDebug1> m_DXGIDebug = nullptr;
	#endif // RHI_DEBUG_ENABLED
	};

#pragma endregion

	inline DynamicRHI_D3D12Impl* GetD3D12RHI()
	{
		RHI_DEV_CHECK( s_DynamicRHI != nullptr && s_DynamicRHI->GetRHIType() == ERHInterfaceType::DirectX12, "Invalid RHI type!" );
		return Cast<DynamicRHI_D3D12Impl*>( s_DynamicRHI );
	}

} // namespace Tridium::D3D12