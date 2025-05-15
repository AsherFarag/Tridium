#pragma once
#include <Tridium/Utils/Log.h>
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
#include <Tridium/Graphics/RHI/RHIFence.h>
#include <Tridium/Graphics/RHI/RHIStateTracker.h>

#include "D3D12.h"
#include "D3D12ToRHI.h"
#include "ThirdParty/D3D12MemAlloc.h"

DECLARE_LOG_CATEGORY( DirectX );

#if RHI_USE_DEBUG_NAMES
	#define D3D12_SET_DEBUG_NAME( _Object, _Name ) \
	do { \
		if ( RHIQuery::IsDebug() && !_Name.empty() ) \
		{ \
			::Tridium::WString wName( _Name.begin(), _Name.end() ); \
			_Object->SetName( wName.c_str() ); \
			::Tridium::D3D12::D3D12Context::Get()->StringStorage.EmplaceBack( std::move( wName ) ); \
		} \
	} while ( false )
#else
	#define D3D12_SET_DEBUG_NAME( _Object, _Name )
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

	using RootParameterIndex = uint32_t;
	constexpr RootParameterIndex c_InvalidRootParameterIndex = ~0u;

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
		size_t Hash = 0;
		InlineArray<Pair<RHIBindingLayoutRef, RootParameterIndex>, RHIConstants::MaxBindingLayouts> Layouts;
		ComPtr<ID3D12RootSignature> D3D12Signature;
		uint32_t InlinedConstantsSize = 0; // Size of the inlined constants in bytes
		RootParameterIndex RootParamInlinedConstants = c_InvalidRootParameterIndex;

		static RootSignature Build( Span<RHIBindingLayoutRef> a_Layouts, bool a_AllowInputLayout, bool a_IsLocal, Span<const D3D12_ROOT_PARAMETER1> a_CustomParams = {} );
	};

	struct D3D12TierInfo
	{
		size_t MaxShaderVisibleDescriptorHeapSize = 0;
		size_t MaxShaderVisibleSamplerHeapSize = 0;
	};

	inline const D3D12TierInfo& TierInfo() { return { D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2, D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE }; }

	TODO( "Implement this or remove it" );
	struct DeferredDeleteObject {};

	//======================================================================
	// Device Child
	//  A base class for an object that is owned by a D3D12 device.
	class DeviceChild
	{
	public:
		NON_COPYABLE( DeviceChild );
		DeviceChild() = delete;
		DeviceChild( ID3D12Device* a_ParentDevice )
			: m_ParentDevice( a_ParentDevice ) {
		}
		DeviceChild( DeviceChild&& other ) noexcept
			: m_ParentDevice( std::move( other.m_ParentDevice ) ) {
		}
		DeviceChild& operator=( DeviceChild&& other ) noexcept
		{
			if ( this != &other )
			{
				m_ParentDevice = std::move( other.m_ParentDevice );
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
		ComPtr<ID3D12Resource> Resource{};

		bool Valid() const { return Allocation && Resource; }

		void Release()
		{
			Allocation.Release();
			Resource.Release();
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
	// D3D12 Command Context
	//  Owns and manages the command queue, command allocator, command list and fence.
	struct CommandContext
	{
		ComPtr<ID3D12CommandQueue> CmdQueue = nullptr;
		ComPtr<ID3D12CommandAllocator> CmdAllocator = nullptr;
		ComPtr<ID3D12GraphicsCommandList> CmdList = nullptr;

		ComPtr<ID3D12Fence1> Fence = nullptr;
		HANDLE FenceEvent = nullptr;
		uint64_t FenceValue = 0;

		~CommandContext()
		{
			if ( CmdList ) CmdList->Release();
			if ( CmdAllocator ) CmdAllocator->Reset();
			if ( CmdQueue ) CmdQueue->Signal( Fence.Get(), FenceValue );
			if ( FenceEvent ) CloseHandle( FenceEvent );
		}

		[[nodiscard]] bool IsFenceComplete( uint64_t a_FenceValue ) const
		{
			return CmdQueue && (Fence->GetCompletedValue() >= a_FenceValue);
		}

		[[nodiscard]] uint64_t Signal()
		{
			if ( CmdQueue )
			{
				CmdQueue->Signal( Fence.Get(), ++FenceValue );
				return FenceValue;
			}
			return 0;
		}

		void Wait( uint64_t a_FenceValue )
		{
			if ( Fence->GetCompletedValue() < a_FenceValue )
			{
				Fence->SetEventOnCompletion( a_FenceValue, FenceEvent );
				WaitForSingleObject( FenceEvent, INFINITE );
			}
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
			m_UploadBuffer.Resource->Map( 0, &range, &mappedData );

			return reinterpret_cast<uint8_t*>(mappedData) + o_Offset;
		}

		void Reset()
		{
			m_CurrentOffset = 0;
		}

		ID3D12Resource* GetResource() const { return m_UploadBuffer.Resource.Get(); }

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

	private:
		DescriptorHeapRef m_Heap;
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

		// Frees the given heap after a frame/s has completed.
		void DeferredFreeHeap( DescriptorHeapRef&& a_Heap );
		// Frees the given heap immediately.
		void ImmediateFreeHeap( DescriptorHeapRef&& a_Heap );

		// Add a heap to the pool so that it can be reused.
		void AddHeapToPool( ComPtr<ID3D12DescriptorHeap>&& a_Heap, ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, EDescriptorHeapFlags a_Flags );

	private:
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
		~DescriptorHeapManager() { Shutdown(); }

		// Initialize the descriptor heap manager
		void Init( ID3D12Device* a_Device, uint32_t a_NumGlobalResourceDescriptors, uint32_t a_NumGlobalSamplerDescriptors );
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
	// FENCE IMPLEMENTATION
	//=======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIFence_D3D12Impl, RHIFence )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIFence_D3D12Impl, ERHInterfaceType::DirectX12 );
		RHIFence_D3D12Impl( const DescriptorType & a_Desc );
		virtual ~RHIFence_D3D12Impl();

		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		uint64_t GetCompletedValue() override;
		void Signal( uint64_t a_Value ) override;
		void Wait( uint64_t a_Value ) override;

	private:
		ComPtr<ID3D12Fence> m_Fence = nullptr;
		HANDLE m_FenceEvent = nullptr;
	};

	//======================================================================
	// TEXTURE IMPLEMENTATION
	//=======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHITexture_D3D12Impl, RHITexture )
	{
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHITexture_D3D12Impl, ERHInterfaceType::DirectX12 );

		RHITexture_D3D12Impl( const RHITextureDescriptor & a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData = {} );
		~RHITexture_D3D12Impl() override = default;

		virtual bool Release() override;
		virtual size_t GetSizeInBytes() const override;
		virtual const void* NativePtr() const override { return Texture.Resource.Get(); }
		virtual bool IsValid() const override { return Texture.Valid(); }

		// D3D12 specific functions
		[[nodiscard]] D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const;

		//bool Write(
		//	ID3D12GraphicsCommandList& a_CmdList, 
		//	uint32_t a_MipLevel, uint32_t a_ArraySlice, Box a_Region, const RHITextureSubresourceData& a_Data );

		bool CopyTexture(
			ID3D12GraphicsCommandList & a_CmdList, RHITexture_D3D12Impl & a_SrcTexture,
			uint32_t a_SrcMipLevel, uint32_t a_SrcArraySlice, Box a_SrcRegion,
			uint32_t a_DstMipLevel, uint32_t a_DstArraySlice, Box a_DstRegion
		);

		D3D12::ManagedResource Texture{};
	};

	//======================================================================
	// BUFFER IMPLEMENTATION
	//=======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBuffer_D3D12Impl, RHIBuffer )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBuffer_D3D12Impl, ERHInterfaceType::DirectX12 );

		RHIBuffer_D3D12Impl( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data = {} );
		virtual bool Release() override { ManagedBuffer.Release(); return true; }
		virtual bool IsValid() const override { return ManagedBuffer.Valid(); }
		virtual size_t GetSizeInBytes() const override { return ManagedBuffer.Allocation->GetSize(); }
		virtual const void* NativePtr() const override { return ManagedBuffer.Resource.Get(); }

		// D3D12 specific functions
		D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const;

		D3D12::ManagedResource ManagedBuffer{};
	};

	//==================================================================
	// SAMPLER IMPLEMENTATION
	//==================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHISampler_D3D12Impl, RHISampler )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHISampler_D3D12Impl, ERHInterfaceType::DirectX12 )
		RHISampler_D3D12Impl( const DescriptorType & a_Desc );

		bool Release() override { SamplerHeap.Release(); SamplerHandle = {}; SamplerDesc = {}; return true; }
		bool IsValid() const override { return SamplerHeap != nullptr; }
		const void* NativePtr() const override { return SamplerHeap.Get(); }

		D3D12_STATIC_SAMPLER_DESC GetStaticSamplerDesc( uint32_t a_ShaderRegister, ERHIShaderVisibility a_ShaderVisibility = ERHIShaderVisibility::All ) const;

		D3D12_SAMPLER_DESC SamplerDesc{};
		ComPtr<ID3D12DescriptorHeap> SamplerHeap;
		D3D12_GPU_DESCRIPTOR_HANDLE SamplerHandle = {};
	};

	//======================================================================
	// BINDING LAYOUT IMPLEMENTATION
	//=======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBindingLayout_D3D12Impl, RHIBindingLayout )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBindingLayout_D3D12Impl, ERHInterfaceType::DirectX12 );
		RHIBindingLayout_D3D12Impl( const DescriptorType & a_Desc );
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		ComPtr<ID3D12RootSignature> m_RootSignature;

		uint32_t InlinedConstantsSize = 0; // Size of the inlined constants in bytes
		RootParameterIndex RootParamInlinedConstants = ~0;
		RootParameterIndex RootParamSRV = ~0;
		RootParameterIndex RootParamSamplers = ~0;
		InlineArray<D3D12_ROOT_PARAMETER1, 32> RootParams;
	};

	//======================================================================
	// BINDING SET IMPLEMENTATION
	//=======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIBindingSet_D3D12Impl, RHIBindingSet )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIBindingSet_D3D12Impl, ERHInterfaceType::DirectX12 );
		RHIBindingSet_D3D12Impl( const DescriptorType & a_Desc );
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;
	};

	//======================================================================
	// SHADER MODULE IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIShaderModule_D3D12Impl, RHIShaderModule )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIShaderModule_D3D12Impl, ERHInterfaceType::DirectX12 )
		RHIShaderModule_D3D12Impl( const DescriptorType & a_Desc );
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		Array<Byte> Bytecode;
	};

	//======================================================================
	// GRAPHICS PIPELINE STATE IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIGraphicsPipelineState_D3D12Impl, RHIGraphicsPipelineState )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIGraphicsPipelineState_D3D12Impl, ERHInterfaceType::DirectX12 );

		RHIGraphicsPipelineState_D3D12Impl( const DescriptorType & a_Desc );
		bool Release() override;
		bool IsValid() const override { return PSO != nullptr; }
		const void* NativePtr() const override { return PSO.Get(); }

		ComPtr<ID3D12PipelineState> PSO;
		D3D12_INPUT_ELEMENT_DESC VertexLayout[RHIConstants::MaxVertexAttributes];
		uint32_t VertexLayoutSize = 0;
	};

	//======================================================================
	// SWAPCHAIN IMPLEMENTATION
	//======================================================================

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHISwapChain_D3D12Impl, RHISwapChain )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHISwapChain_D3D12Impl, ERHInterfaceType::DirectX12 );

		RHISwapChain_D3D12Impl( const DescriptorType & a_Desc );
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override { return SwapChain.Get(); }
		bool Present() override;
		RHITextureRef GetBackBuffer() override;
		bool Resize( uint32_t a_Width, uint32_t a_Height ) override;
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		ComPtr<IDXGISwapChain3> SwapChain;
		InlineArray<RHITextureRef, RHIConstants::MaxColorTargets> RTVs;

	private:
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

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHICommandList_D3D12Impl, RHICommandList )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHICommandList_D3D12Impl, ERHInterfaceType::DirectX12 );

		RHICommandList_D3D12Impl( const RHICommandListDescriptor & a_Desc );
		virtual bool Release() override;
		virtual bool IsValid() const override { return CommandList != nullptr; }
		virtual const void* NativePtr() const override { return CommandList.Get(); }

		virtual bool SetGraphicsCommands( const RHIGraphicsCommandBuffer & a_CmdBuffer ) override;
		virtual bool SetComputeCommands( const RHIComputeCommandBuffer & a_CmdBuffer ) override;
		virtual bool IsCompleted() const override;
		virtual void WaitUntilCompleted() override;

		ID3D12GraphicsCommandList* GraphicsCommandList() const
		{
			RHI_DEV_CHECK( m_Desc.QueueType == ERHICommandQueueType::Graphics, "Command list is not a graphics command list!" );
			return static_cast<ID3D12GraphicsCommandList*>(CommandList.Get());
		}

		ComPtr<ID3D12CommandList> CommandList{};
		RHIBindingLayout* CurrentSBL = nullptr;

	private:
		RHIResourceStateTracker m_ResourceStateTracker{};
		// Reset after each execution
		struct State
		{
			Array<D3D12::ManagedResource> D3D12Resources{};
			Array<D3D12_RESOURCE_BARRIER> D3D12Barriers{};
			Array<D3D12::DescriptorHeapRef> Heaps{};
			D3D12::DescriptorHeapRef LastRTVHeap{};
			D3D12::DescriptorHeapRef LastDSVHeap{};
			uint32_t ShaderInputOffset = 0;

			struct GraphicsState
			{
				SharedPtr<RHIGraphicsPipelineState_D3D12Impl> PSO{};
				Array<RHITexture*> CurrentRTs{};
				RHITexture* CurrentDSV{};
			} Graphics{};

			struct ComputeState
			{
			} Compute{};

			void Clear()
			{
				D3D12Resources.Clear();
				D3D12Barriers.Clear();
				Heaps.Clear();
				LastRTVHeap = nullptr;
				LastDSVHeap = nullptr;
				ShaderInputOffset = 0;
				Graphics.PSO = nullptr;
			}
		} m_State{};

		uint64_t m_FenceValue = 0;

	private:
		void CommitBarriers();

		// Commands

		void SetBindingLayout( const RHICommand::SetBindingLayout & a_Data );
		void SetShaderBindings( const RHICommand::SetShaderBindings & a_Data );
		void SetInlinedConstants( const RHICommand::SetInlinedConstants & a_Data );
		void ResourceBarrier( const RHICommand::ResourceBarrier & a_Data );
		void UpdateBuffer( const RHICommand::UpdateBuffer & a_Data );
		void CopyBuffer( const RHICommand::CopyBuffer & a_Data );
		void UpdateTexture( const RHICommand::UpdateTexture & a_Data );
		void CopyTexture( const RHICommand::CopyTexture & a_Data );

		// Graphics
		void SetGraphicsPipelineState( const RHICommand::SetGraphicsPipelineState & a_Data );
		void SetRenderTargets( const RHICommand::SetRenderTargets & a_Data );
		void ClearRenderTargets( const RHICommand::ClearRenderTargets & a_Data );
		void SetScissors( const RHICommand::SetScissors & a_Data );
		void SetViewports( const RHICommand::SetViewports & a_Data );
		void SetIndexBuffer( const RHICommand::SetIndexBuffer & a_Data );
		void SetVertexBuffer( const RHICommand::SetVertexBuffer & a_Data );
		void SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology & a_Data );
		void Draw( const RHICommand::Draw & a_Data );
		void DrawIndexed( const RHICommand::DrawIndexed & a_Data );

		// Compute
		void SetComputePipelineState( const RHICommand::SetComputePipelineState & a_Data );
		void DispatchCompute( const RHICommand::DispatchCompute & a_Data );
		void DispatchComputeIndirect( const RHICommand::DispatchComputeIndirect & a_Data );
	};

	//======================================================================
	// DYNAMIC RHI IMPLEMENTATION
	//======================================================================

	class DynamicRHI_D3D12Impl final : public IDynamicRHI
	{
	public:
		//==============================================
		// Core RHI functions
		// Initialise the RHI with the given configuration.
		virtual bool Init( const RHIConfig& a_Config ) override;
		// Shutdown the RHI.
		virtual bool Shutdown() override;
		// Execute the given command list.
		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) override;
		// Returns the type of the Dynamically bound RHI.
		virtual ERHInterfaceType GetRHIType() const override { return ERHInterfaceType::DirectX12; }
		// Returns the static RHI type.
		static constexpr ERHInterfaceType GetStaticRHIType() { return ERHInterfaceType::DirectX12; }
		//==============================================

		//=====================================================
		// Resource creation
		virtual RHIFenceRef CreateFence( const RHIFenceDescriptor& a_Desc ) override;
		virtual RHISamplerRef CreateSampler( const RHISamplerDescriptor& a_Desc ) override;
		virtual RHITextureRef CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData ) override;
		virtual RHIBufferRef CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data ) override;
		virtual RHIGraphicsPipelineStateRef CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc ) override;
		virtual RHICommandListRef CreateCommandList( const RHICommandListDescriptor& a_Desc ) override;
		virtual RHIShaderModuleRef CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc ) override;
		virtual RHIBindingLayoutRef CreateBindingLayout( const RHIBindingLayoutDescriptor& a_Desc ) override;
		virtual RHIBindingSetRef CreateBindingSet( const RHIBindingSetDescriptor& a_Desc ) override;
		virtual RHISwapChainRef CreateSwapChain( const RHISwapChainDescriptor& a_Desc ) override;
		//=====================================================

		//====================================================
		// D3D12 Specific
		//====================================================

		auto& GetCommandContext( ERHICommandQueueType a_Type )
		{
			ASSERT( a_Type < ERHICommandQueueType::COUNT, "Invalid command queue type!" );
			return m_CmdContexts[static_cast<size_t>( a_Type )];
		}

		const auto& GetCommandContext( ERHICommandQueueType a_Type ) const
		{
			ASSERT( a_Type < ERHICommandQueueType::COUNT, "Invalid command queue type!" );
			return m_CmdContexts[static_cast<size_t>( a_Type )];
		}

		const auto& GetDXGIFactory() const { return m_DXGIFactory; }
		const auto& GetDXGIAdapter() const { return m_DXGIAdapter; }
		const auto& GetAllocator() const { return m_Allocator; }
		const auto& GetUploadBuffer() const { return m_UploadBuffer; }
		auto& GetUploadBuffer() { return m_UploadBuffer; }
		size_t GetNextCommandListIndex() { return m_NextCommandListIndex; }

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

		D3D12::DescriptorHeapManager& GetDescriptorHeapManager() { return m_DescriptorHeapManager; }
		const D3D12::DescriptorHeapManager& GetDescriptorHeapManager() const { return m_DescriptorHeapManager; }

		template<typename T>
		void DeferredDelete( const T& a_Object )
		{
			LOG( LogCategory::DirectX, Debug, "Deferred delete object." );
			m_ObjectsToDelete.EmplaceBack();
		}

		//====================================================

	private:
		ComPtr<IDXGIAdapter> m_DXGIAdapter = nullptr;
		ComPtr<IDXGIFactory> m_DXGIFactory = nullptr;
		ComPtr<ID3D12Device> m_Device = nullptr;
		ComPtr<D3D12MA::Allocator> m_Allocator = nullptr;
		DescriptorHeapManager m_DescriptorHeapManager{};

		uint32_t m_MaxD3D12DeviceVersion = 0;

		size_t m_NextCommandListIndex = 0;
		Array<DeferredDeleteObject> m_ObjectsToDelete{};
		UploadBuffer m_UploadBuffer{};
		FixedArray<CommandContext, size_t( ERHICommandQueueType::COUNT )> m_CmdContexts{};

		//=====================================================

	#if RHI_DEBUG_ENABLED
	public:
		virtual void DumpDebug() override;

	private:
		ComPtr<ID3D12Debug> m_D3D12Debug = nullptr;
		ComPtr<IDXGIDebug1> m_DXGIDebug = nullptr;
	#endif
	};

#pragma endregion

	inline DynamicRHI_D3D12Impl* GetD3D12RHI()
	{
		RHI_DEV_CHECK( s_DynamicRHI != nullptr && s_DynamicRHI->GetRHIType() == ERHInterfaceType::DirectX12, "Invalid RHI type!" );
		return Cast<DynamicRHI_D3D12Impl*>( s_DynamicRHI );
	}

} // namespace Tridium::D3D12