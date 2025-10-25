#include "UUID.h"
#include <random>

namespace Tridium {

    UUID UUID::Generate()
    {
        static thread_local std::mt19937_64 rng{ std::random_device{}( ) };
        std::uniform_int_distribution<uint64_t> dist;

        uint64_t high = dist( rng );
        uint64_t low = dist( rng );

        // Set the version (UUID v4 = random) and variant bits per RFC 4122
        // version: bits 12–15 of time_hi_and_version
        high &= 0xFFFFFFFFFFFF0FFFULL;
        high |= 0x0000000000004000ULL;

        // variant: bits 6–7 of clock_seq_hi_and_reserved
        low &= 0x3FFFFFFFFFFFFFFFULL;
        low |= 0x8000000000000000ULL;

        return UUID( high, low );
    }

} // namespace Tridium