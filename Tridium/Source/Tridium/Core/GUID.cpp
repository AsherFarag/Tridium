#include "tripch.h"
#include "GUID.h"
#include <random>
#include <cstdint>

namespace Tridium {

#define GENERATE_RANDOM_ID( Type )                            \
		template<>                                            \
		Type GenerateRandomID<Type>()                         \
		{                                                     \
			static std::random_device rd;                     \
			static std::mt19937_64 gen( rd() );               \
			static std::uniform_int_distribution<Type> dis;   \
			return dis( gen );                                \
		}

	GENERATE_RANDOM_ID( int16_t );
	GENERATE_RANDOM_ID( int32_t );
	GENERATE_RANDOM_ID( int64_t );

	GENERATE_RANDOM_ID( uint16_t );
	GENERATE_RANDOM_ID( uint32_t );
	GENERATE_RANDOM_ID( uint64_t );
}