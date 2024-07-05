#include "tripch.h"
#include "GUID.h"
#include <random>
#include <cstdint>

namespace Tridium {

    GUID GUID::Create()
    {
        // Seed with a real random value, if available
        static std::random_device rd;

        // Use the Mersenne Twister engine
        static std::mt19937_64 gen( rd() );

        // Define the distribution range for uint64_t
        static std::uniform_int_distribution<uint64_t> dis;

        GUID id( dis( gen ) );

        return id.Valid() ? id : Create();
    }
}