#pragma once
#include "Cast.h"

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

    using int8_t   = signed char;
    using int16_t  = short;
    using int32_t  = int;
    using int64_t  = long long;
    using uint8_t  = unsigned char;
    using uint16_t = unsigned short;
    using uint32_t = unsigned int;
    using uint64_t = unsigned long long;

    using float32_t = float;
    using float64_t = double;

    using Byte = uint8_t;
}