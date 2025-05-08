#include "tripch.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12Common.h"
#include "D3D12DynamicRHI.h"
#include "Utils/ThirdParty/D3D12MemAlloc.h"

namespace Tridium::D3D12 {

	//////////////////////////////////////////////////////////////////////////
	// DescriptorHeap
	//////////////////////////////////////////////////////////////////////////

    DescriptorHeap::DescriptorHeap( ID3D12Device* a_Device, ComPtr<ID3D12DescriptorHeap>&& a_Heap,
        uint32_t a_NumDescriptors, ERHIDescriptorHeapType a_Type,
        ED3D12DescriptorHeapFlags a_Flags, bool a_IsGlobal )
		: DeviceChild( a_Device )
		, m_Heap( std::move( a_Heap ) )
		, m_Type( a_Type )
		, m_Flags( a_Flags )
		, m_CPUBase( m_Heap->GetCPUDescriptorHandleForHeapStart() )
		, m_GPUBase( m_Heap->GetGPUDescriptorHandleForHeapStart() )
		, m_IsGlobal( a_IsGlobal )
		, m_IsSuballocation( false )
		, m_Offet( 0u )
		, m_NumDescriptors( a_NumDescriptors )
		, m_DescriptorSize( a_Device->GetDescriptorHandleIncrementSize( Translate( a_Type ) ) )
    {
    }

	DescriptorHeap::DescriptorHeap( DescriptorHeap& a_ParentHeap, uint32_t a_Offset, uint32_t a_NumDescriptors )
		: DeviceChild( a_ParentHeap.ParentDevice() )
		, m_Heap( a_ParentHeap.m_Heap )
		, m_Type( a_ParentHeap.m_Type )
		, m_Flags( a_ParentHeap.m_Flags )
		, m_CPUBase( a_ParentHeap.GetCPUHandle( a_Offset ) )
		, m_GPUBase( a_ParentHeap.GetGPUHandle( a_Offset ) )
		, m_IsGlobal( a_ParentHeap.m_IsGlobal )
		, m_IsSuballocation( true )
		, m_Offet( a_Offset )
		, m_NumDescriptors( a_NumDescriptors )
		, m_DescriptorSize( a_ParentHeap.m_DescriptorSize )
	{
	}

	DescriptorHeap::~DescriptorHeap()
	{
		if ( IsPoolable() )
		{
			// Return the heap to the pool.
			GetD3D12RHI()->GetDescriptorHeapManager().AddHeapToPool( std::move( m_Heap ), m_Type, m_NumDescriptors, m_Flags );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// DescriptorHeapManager
	//////////////////////////////////////////////////////////////////////////

	// Helper function
	static DescriptorHeapRef CreateDescriptorHeap( Device* a_Device, uint32_t a_NumDescriptors, ERHIDescriptorHeapType a_Type, ED3D12DescriptorHeapFlags a_Flags, bool a_IsGlobal, StringView a_DebugName = StringView{} )
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = Translate( a_Type );
		desc.NumDescriptors = a_NumDescriptors;
		desc.Flags = Translate( a_Flags );
		desc.NodeMask = 0;
		ComPtr<ID3D12DescriptorHeap> heap;
		HRESULT hr = a_Device->CreateDescriptorHeap( &desc, IID_PPV_ARGS( &heap ) );
		ENSURE( SUCCEEDED( hr ), "Failed to create descriptor heap!" );

		D3D12_SET_DEBUG_NAME( heap.Get(), a_DebugName );
		LOG( LogCategory::RHI, Debug, "Heap Created - Name: {0}, Type: {1}, NumDescriptors: {2}",
			a_DebugName, RHIDescriptorHeapTypeToString( a_Type ), a_NumDescriptors );

		return MakeShared<DescriptorHeap>( a_Device, std::move( heap ), a_NumDescriptors, a_Type, a_Flags, a_IsGlobal );
	}


	DescriptorHeapRef DescriptorHeapManager::AllocateIndependentHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, ED3D12DescriptorHeapFlags a_Flags, StringView a_DebugName )
	{
		// If this heap is poolable, attempt to find one in the pool.
		if ( EnumFlags( a_Flags ).HasFlag( ED3D12DescriptorHeapFlags::Poolable ) )
		{
			if ( ComPtr<ID3D12DescriptorHeap> heap = AcquirePooledHeap( a_Type, a_NumDescriptors, a_Flags ) )
			{
				return MakeShared<DescriptorHeap>( m_Device, std::move( heap ), a_NumDescriptors, a_Type, a_Flags, false );
			}
		}

		// No valid heaps were available, create a new one
		return CreateDescriptorHeap( m_Device, a_NumDescriptors, a_Type, a_Flags, false, a_DebugName );
	}

	DescriptorHeapRef DescriptorHeapManager::AllocateHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, ED3D12DescriptorHeapFlags a_Flags, StringView a_DebugName )
	{
		for ( auto& globalHeap : m_GlobalHeaps )
		{
			// Search for a global heap that can allocate this descriptor heap

			if ( !globalHeap.HandlesAllocation( a_Type, a_Flags ) )
			{
				continue;
			}

			uint32_t offset = 0;
			if ( globalHeap.Allocate( a_NumDescriptors, offset ) )
			{
				return MakeShared<DescriptorHeap>( *( globalHeap.Heap() ), offset, a_NumDescriptors );
			}

			TODO( "Handle if the global heap is full." );
			ASSERT( false, "Global heap is full!" );
			return nullptr;
		}

		// Since we could not find a global heap to use, create a new independent heap
		return AllocateIndependentHeap( a_Type, a_NumDescriptors, a_Flags, a_DebugName );
	}

	void DescriptorHeapManager::DeferredFreeHeap( DescriptorHeapRef&& a_Heap )
	{

		GetD3D12RHI()->DeferredDelete( std::move( a_Heap ) );
	}

	void DescriptorHeapManager::ImmediateFreeHeap( DescriptorHeapRef&& a_Heap )
	{
		if ( !a_Heap->IsGlobal() )
		{
			// Destroy the heap
			ENSURE( a_Heap.use_count() == 1, "Heap is still in use! " );
			a_Heap.reset();
			return;
		}

		for ( auto& globalHeap : m_GlobalHeaps )
		{
			if ( globalHeap.IsHeapAChild( *a_Heap ) )
			{
				globalHeap.Free( a_Heap->Offset(), a_Heap->NumDescriptors() );
				a_Heap.reset();
				return;
			}
		}
	}

	void DescriptorHeapManager::AddHeapToPool( ComPtr<ID3D12DescriptorHeap>&& a_Heap, ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, ED3D12DescriptorHeapFlags a_Flags )
	{
		std::lock_guard lock( m_PooledHeapsMutex );
		m_PooledHeaps.EmplaceBack( std::move( a_Heap ), a_Type, a_NumDescriptors, a_Flags );
	}

	void DescriptorHeapManager::Init( Device* a_Device, uint32_t a_NumGlobalResourceDescriptors, uint32_t a_NumGlobalSamplerDescriptors )
	{
		ENSURE( a_Device, "Device is null!" );
		m_Device = a_Device;

		m_GlobalHeaps = Array<DescriptorManager>(); // ( m_GlobalHeaps was not initialized properly )
		m_GlobalHeaps.Reserve( 2 );

		if ( a_NumGlobalResourceDescriptors > 0 )
		{
			DescriptorHeapRef descHeap = CreateDescriptorHeap( 
				m_Device,
				a_NumGlobalResourceDescriptors,
				ERHIDescriptorHeapType::RenderResource,
				ED3D12DescriptorHeapFlags::GPUVisible, 
				true, "GlobalResourceDescriptorHeap" );

			m_GlobalHeaps.EmplaceBack( m_Device, std::move( descHeap ) );
		}

		if ( a_NumGlobalSamplerDescriptors > 0 )
		{
			DescriptorHeapRef samplerHeap = CreateDescriptorHeap(
				m_Device,
				a_NumGlobalSamplerDescriptors,
				ERHIDescriptorHeapType::Sampler,
				ED3D12DescriptorHeapFlags::GPUVisible,
				true, "GlobalSamplerDescriptorHeap" );
			m_GlobalHeaps.EmplaceBack( m_Device, std::move( samplerHeap ) );
		}
	}

	void DescriptorHeapManager::Shutdown()
	{
	}

	ComPtr<ID3D12DescriptorHeap> DescriptorHeapManager::AcquirePooledHeap( ERHIDescriptorHeapType a_Type, uint32_t a_NumDescriptors, ED3D12DescriptorHeapFlags a_Flags )
	{
		// Attempt to find a valid heap, if one is found, remove it from the pool and return it.
		std::lock_guard lock( m_PooledHeapsMutex );
		for ( auto it = m_PooledHeaps.Begin(); it != m_PooledHeaps.End(); ++it )
		{
			auto& pooledHeap = *it;
			if ( pooledHeap.Type == a_Type && pooledHeap.Flags == a_Flags && pooledHeap.NumDescriptors >= a_NumDescriptors )
			{
				ComPtr<ID3D12DescriptorHeap> heap = pooledHeap.Heap;
				m_PooledHeaps.Erase( it );
				return heap;
			}
		}

		return nullptr;
	}

} // namespace Tridium::D3D12