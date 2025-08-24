#include "tripch.h"
#include "RHIDescriptorAllocator.h"
#include <algorithm> // Binary Search

namespace Tridium {

	//////////////////////////////////////////////////////////////////////////
	// RHIDescriptorAllocator
	//////////////////////////////////////////////////////////////////////////

	RHIDescriptorAllocator& RHIDescriptorAllocator::operator=( RHIDescriptorAllocator&& a_Other ) noexcept
	{
		if ( this != &a_Other )
		{
			m_Ranges = std::move( a_Other.m_Ranges );
			m_Capacity = a_Other.m_Capacity;
		}
		return *this;
	}

	void RHIDescriptorAllocator::Init( uint32_t a_Capacity )
	{
		m_Capacity = a_Capacity;
		m_Ranges.EmplaceBack( 0u, m_Capacity - 1 );
	}

	void RHIDescriptorAllocator::Shutdown()
	{
		m_Ranges.Clear();
		m_Capacity = 0u;
	}

	RHIDescriptorHandle RHIDescriptorAllocator::Allocate( ERHIDescriptorHeapType a_Type )
	{
		uint32_t offset = 0xFFFFFFFFu;
		if ( !Allocate( 1u, offset ) )
		{
			return RHIDescriptorHandle{};
		}

		return RHIDescriptorHandle( a_Type, offset );
	}

	bool RHIDescriptorAllocator::Allocate( uint32_t a_Count, uint32_t& o_BaseIndex )
	{
		std::lock_guard<std::mutex> lock( m_Mutex );

		// Set to an invalid index
		o_BaseIndex = 0xFFFFFFFFu;

		if ( m_Ranges.Empty() )
		{
			return false;
		}

		// Use a binary search to find the first range that fits
		auto it = std::lower_bound(
			m_Ranges.Begin(), m_Ranges.End(), a_Count,
			[]( const RHIDescriptorAllocatorRange& a_Range, uint32_t a_Count ) -> bool
			{
				return a_Range.Size() < a_Count;
			}
		);

		if ( it == m_Ranges.End() )
		{
			return false;
		}

		RHIDescriptorAllocatorRange& range = *it;
		o_BaseIndex = range.First;

		if ( a_Count == range.Size() )
		{
			// Remove the range if it is fully allocated
			m_Ranges.Erase( it );
		}
		else
		{
			// Shrink the range if it is partially allocated
			range.First += a_Count;
		}

		return true;
	}

	void RHIDescriptorAllocator::Free( RHIDescriptorHandle a_Handle )
	{
		if ( a_Handle.Valid() )
		{
			Free( a_Handle.Index, 1u );
		}
	}

	void RHIDescriptorAllocator::Free( uint32_t a_BaseIndex, uint32_t a_Count )
	{
		std::lock_guard<std::mutex> lock( m_Mutex );

		uint32_t newFirst = a_BaseIndex;
		uint32_t newLast = a_BaseIndex + a_Count - 1;

		auto it = std::lower_bound(
			m_Ranges.Begin(), m_Ranges.End(), newFirst,
			[]( const RHIDescriptorAllocatorRange& range, uint32_t value )
			{
				return range.Last < value;
			} );

		// Try merge with previous range
		if ( it != m_Ranges.Begin() )
		{
			auto prevIt = std::prev( it );
			if ( prevIt->Last + 1 >= newFirst ) // overlap or adjacent
			{
				prevIt->Last = std::max( prevIt->Last, newLast );

				// Also check if it now merges with the next range
				if ( it != m_Ranges.End() && prevIt->Last + 1 >= it->First )
				{
					prevIt->Last = std::max( prevIt->Last, it->Last );
					m_Ranges.Erase( it );
				}

				return;
			}
		}

		// Try merge with current range
		if ( it != m_Ranges.End() && newLast + 1 >= it->First )
		{
			it->First = std::min( it->First, newFirst );
			it->Last = std::max( it->Last, newLast );
			return;
		}

		// Otherwise insert as a new free block
		m_Ranges.Insert( it, RHIDescriptorAllocatorRange( newFirst, newLast ) );
	}



	//////////////////////////////////////////////////////////////////////////
	// RHIHeapDescriptorAllocator
	//////////////////////////////////////////////////////////////////////////

	RHIHeapDescriptorAllocator& RHIHeapDescriptorAllocator::operator=( RHIHeapDescriptorAllocator&& a_Other ) noexcept
	{
		if ( this != &a_Other )
		{
			RHIDescriptorAllocator::operator=( std::move( a_Other ) );
			m_Type = a_Other.m_Type;
		}
		return *this;
	}

} // namespace Tridium
