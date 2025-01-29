#pragma once
#include <stdint.h>
#include <limits.h>

#define NON_COPYABLE( Type )\
    Type( const Type& ) = delete;\
    Type& operator=( const Type& ) = delete;

#define NON_MOVABLE( Type )\
    Type( Type&& ) = delete;\
    Type& operator=( Type&& ) = delete;

#define NON_COPYABLE_OR_MOVABLE( Type )\
    NON_COPYABLE( Type )\
    NON_MOVABLE( Type )

namespace Tridium {

	using Byte = uint8_t;
	using float32_t = float;
	using float64_t = double;
	using hash_t = uint32_t;
}