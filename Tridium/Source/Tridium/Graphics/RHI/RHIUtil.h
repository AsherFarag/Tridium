#pragma once
#include "RHICommon.h"

namespace Tridium {

	namespace RHIUtil {

		namespace Detail {
			template<typename T>
			concept HasSizeMethod = requires(T t) { t.size(); } || requires(T t) { t.Size(); };

			template<HasSizeMethod T>
			[[nodiscard]] auto GetArraySize( const T & a_Array )
			{
				if constexpr ( requires { a_Array.size(); } )
				{
					return a_Array.size();
				}
				else if constexpr ( requires { a_Array.Size(); } )
				{
					return a_Array.Size();
				}
			}
		}

		template<Detail::HasSizeMethod T, Detail::HasSizeMethod U>
		[[nodiscard]] bool CompareArrays(const T& a_A, const U& a_B)
		{
			const auto sizeA = Detail::GetArraySize( a_A );
			const auto sizeB = Detail::GetArraySize( a_B );

			if (sizeA != sizeB)
				return false;

			for (size_t i = 0; i < sizeA; ++i)
			{
				if (a_A[i] != a_B[i])
				{
					return false;
				}
			}

			return true;
		}


		template<Detail::HasSizeMethod T, Detail::HasSizeMethod U, typename _SizeType = uint32_t>
		[[nodiscard]] _SizeType ArrayDifferenceMask( const T& a_A, const U& a_B )
		{
			const auto sizeA = Detail::GetArraySize( a_A );
			const auto sizeB = Detail::GetArraySize( a_B );

			ASSERT( sizeA <= sizeof( _SizeType ), "Array size exceeds bitmask size" );
			ASSERT( sizeB <= sizeof( _SizeType ), "Array size exceeds bitmask size" );

			if ( sizeA != sizeB )
				return _SizeType( ~0 ); // Return all bits set if sizes differ

			_SizeType mask = 0;
			for ( _SizeType i = 0; i < _SizeType( sizeA ); i++ )
			{
				if ( a_A[i] != a_B[i] )
					mask |= (1 << i);
			}

			return mask;
		}
	}

}