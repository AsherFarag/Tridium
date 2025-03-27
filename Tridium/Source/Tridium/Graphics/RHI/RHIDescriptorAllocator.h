#pragma once
#include "RHICommon.h"
#include <mutex>

namespace Tridium {

	//=======================================================
	// Descriptor Allocator Range
	//  Represents a range of descriptors in a descriptor heap.
	struct RHIDescriptorAllocatorRange
	{
		uint32_t First;
		uint32_t Last;

		constexpr uint32_t Size() const { return Last - First; }
		explicit constexpr RHIDescriptorAllocatorRange( uint32_t a_First, uint32_t a_Last )
			: First( a_First ), Last( a_Last ) {}
	};

	//=======================================================
	// Descriptor Allocator
	//  Default base class for allocating and managing ranges of descriptors.
	//  A single large allocation is split into smaller ranges when descriptors are allocated.
	//  Allocation ranges are merged when descriptors are freed.
	// Example: Allocating 16 descriptors, split up into multiple ranges of different sizes.
	//     Range 1           Range 3     Range 5
	//        |                 |           |
	//      | X X X, X X X X X, X X, X X X, X, X X | <- Descriptor Heap (Capacity == 16)
	//               |               |         |
	//            Range 2         Range 4   Range 6
	//
	// NOTE: This class is not an RHIResource and is not required for user level code.
	//		 It is used by RHI implementations that require descriptor management (e.g. D3D12).
	class RHIDescriptorAllocator
	{
	public:
		RHIDescriptorAllocator() = default;
		RHIDescriptorAllocator( uint32_t a_Capacity ) { Init( a_Capacity ); }
		RHIDescriptorAllocator( RHIDescriptorAllocator&& a_Other ) noexcept
			: m_Ranges( std::move( a_Other.m_Ranges ) ), m_Capacity( a_Other.m_Capacity ) {}
		RHIDescriptorAllocator& operator=( RHIDescriptorAllocator&& a_Other ) noexcept;
		virtual ~RHIDescriptorAllocator() = default;

		void Init( uint32_t a_Capacity );
		void Shutdown();

		// Allocate a single descriptor of the specified type
		RHIDescriptorHandle Allocate( ERHIDescriptorHeapType a_Type );
		// Allocate a range of descriptors, o_BaseIndex is set to the first index in the range
		bool Allocate( uint32_t a_Count, uint32_t& o_BaseIndex );
		// Free a single descriptor handle
		void Free( RHIDescriptorHandle a_Handle );
		// Free a range of descriptors from the first index
		void Free( uint32_t a_BaseIndex, uint32_t a_Count );

		uint32_t Capacity() const { return m_Capacity; }

	private:
		Array<RHIDescriptorAllocatorRange> m_Ranges{};
		uint32_t m_Capacity = 0u;
		std::mutex m_Mutex{};
	};

	//============================================================
	// Heap Descriptor Allocator
	//  A Descriptor Allocator with a specified type of descriptor heap.
	// 
	// NOTE: This class is not an RHIResource and is not required for user level code.
	//		 It is used by RHI implementations that require descriptor management (e.g. D3D12).
	class RHIHeapDescriptorAllocator : protected RHIDescriptorAllocator
	{
	public:
		RHIHeapDescriptorAllocator( ERHIDescriptorHeapType a_Type, uint32_t a_Count )
			: RHIDescriptorAllocator( a_Count ), m_Type( a_Type ) {}
		RHIHeapDescriptorAllocator( RHIHeapDescriptorAllocator&& a_Other ) noexcept
			: RHIDescriptorAllocator( std::move( a_Other ) ), m_Type( a_Other.m_Type ) {}
		RHIHeapDescriptorAllocator& operator=( RHIHeapDescriptorAllocator&& a_Other ) noexcept;

		// Allocate a single descriptor
		RHIDescriptorHandle Allocate() { return RHIDescriptorAllocator::Allocate( m_Type ); }
		// Allocate a range of descriptors, o_BaseIndex is set to the first index in the range
		bool Allocate( uint32_t a_Count, uint32_t& o_BaseIndex ) { return RHIDescriptorAllocator::Allocate( a_Count, o_BaseIndex ); }
		// Free a single descriptor handle
		void Free( RHIDescriptorHandle a_Handle ) { RHIDescriptorAllocator::Free( a_Handle ); }
		// Free a range of descriptors from the first index
		void Free( uint32_t a_BaseIndex, uint32_t a_Count ) { RHIDescriptorAllocator::Free( a_BaseIndex, a_Count ); }

		using RHIDescriptorAllocator::Capacity;
		ERHIDescriptorHeapType Type() const { return m_Type; }

		// Returns true if this allocator handles the given descriptor heap type.
		bool HandlesAllocation( ERHIDescriptorHeapType a_Type ) const { return m_Type == a_Type; }

	private:
		ERHIDescriptorHeapType m_Type;
	};

} // namespace Tridium