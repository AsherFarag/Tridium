#include "tripch.h"
#include "RHIDescriptorAllocator.h"

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
		o_BaseIndex = 0xFFFFFFFFu;

		const uint32_t numRanges = static_cast<uint32_t>( m_Ranges.Size() );
		if ( numRanges == 0u )
		{
			return false;
		}

		// Find the first range that can fit the requested number of descriptors
		uint32_t rangeIndex = 0u;
		for ( auto it = m_Ranges.Begin(); it != m_Ranges.End(); ++it )
		{
			RHIDescriptorAllocatorRange& range = *it;
			const uint32_t size = range.Size();
			if ( a_Count <= size )
			{
				o_BaseIndex = range.First;
				if ( a_Count == size && rangeIndex < numRanges - 1 )
				{
					// Remove the range if it's full
					m_Ranges.Erase( it );
				}
				else
				{
					range.First += a_Count;
				}
				return true;
			}

			++rangeIndex;
		}

		return false;
	}

	void RHIDescriptorAllocator::Free( RHIDescriptorHandle a_Handle )
	{
		if ( !a_Handle.Valid() )
		{
			Free( a_Handle.Index, 1u );
		}
	}

	void RHIDescriptorAllocator::Free( uint32_t a_BaseIndex, uint32_t a_Count )
	{
		std::lock_guard<std::mutex> lock( m_Mutex );

		uint32_t newFirst = a_BaseIndex;
		uint32_t newLast = a_BaseIndex + a_Count - 1;
		
		TODO( "Implement a binary search instead" );
		for ( auto it = m_Ranges.Begin(); it != m_Ranges.End(); ++it )
		{
			RHIDescriptorAllocatorRange& range = *it;

			// Merge with existing range (left-side)
			if ( newLast + 1 == range.First )
			{
				range.First = newFirst;
				return;
			}

			// Merge with existing range (right-side)
			if ( newFirst == range.Last + 1 )
			{
				range.Last = newLast;

				// Check if it merges with the next range as well
				auto nextIt = std::next( it );
				if ( nextIt != m_Ranges.End() && range.Last + 1 == nextIt->First )
				{
					range.Last = nextIt->Last;
					m_Ranges.Erase( nextIt ); // Remove next range, as they are merged
				}
				return;
			}

			// Insert new free range before the first larger range. ( An unlikely case )
			if ( newFirst < range.First )
			{
				m_Ranges.Insert( it, RHIDescriptorAllocatorRange( newFirst, newLast ) );
				return;
			}
		}

		// If no existing ranges matched, append at the end
		m_Ranges.EmplaceBack( newFirst, newLast );
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
