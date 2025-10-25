#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Utils/Format.h>
#include <Tridium/Utils/TypeTraits.h>

namespace Tridium {

	//=================================================================================================
	// UUID: A universally unique identifier (UUID) is a 128-bit number used to uniquely identify
	// objects or entities.
	// UUIDs are randomly generated via UUID::Generate().
	//=================================================================================================
	struct UUID
	{
		//=============================================================================================
		union
		{
			uint8_t Data[16];

			struct
			{
				uint64_t High;
				uint64_t Low;
			};
		};

		//=============================================================================================
		constexpr UUID() noexcept : High( 0 ), Low( 0 ) {}

		//=============================================================================================
		constexpr UUID( uint64_t a_High, uint64_t a_Low ) noexcept : High( a_High ), Low( a_Low ) {}

		//=============================================================================================
		static UUID Generate();

		//=============================================================================================
		constexpr bool Valid() const noexcept { return High != 0 || Low != 0; }
		constexpr operator bool() const noexcept { return Valid(); }

		//=============================================================================================
		constexpr bool operator==( const UUID& a_Other ) const noexcept
		{
			return High == a_Other.High && Low == a_Other.Low;
		}

		//=============================================================================================
		constexpr bool operator!=( const UUID& a_Other ) const noexcept
		{
			return !operator==( a_Other );
		}

		//=============================================================================================
		constexpr bool operator<( const UUID& a_Other ) const noexcept
		{
			return ( High < a_Other.High ) || ( High == a_Other.High && Low < a_Other.Low );
		}

		//=============================================================================================
		constexpr bool operator>( const UUID& a_Other ) const noexcept
		{
			return a_Other.operator<( *this );
		}

		//=============================================================================================
		constexpr bool operator<=( const UUID& a_Other ) const noexcept
		{
			return !operator>( a_Other );
		}

		//=============================================================================================
		constexpr bool operator>=( const UUID& a_Other ) const noexcept
		{
			return !operator<( a_Other );
		}

	};

	template<>
	struct Hash<UUID>
	{
		[[nodiscard]] constexpr size_t operator()( const UUID& a_UUID ) const noexcept
		{
			size_t hash = 2166136261U;
			const uint8_t* data = a_UUID.Data;
			for ( size_t i = 0; i < sizeof( UUID ); ++i )
			{
				hash ^= data[i];
				hash *= 16777619U;
			}
			return hash;
		}
	};

} // namespace Tridium

namespace std {

	template<>
	struct formatter<Tridium::UUID> : formatter<string_view>
	{
		template<typename FormatContext>
		auto format( const Tridium::UUID& a_UUID, FormatContext& a_Context ) const
		{
			const auto& data = a_UUID.Data;
			return formatter<string_view>::format(
				std::format(
					"{:02X}{:02X}{:02X}{:02X}-"
					"{:02X}{:02X}-"
					"{:02X}{:02X}-"
					"{:02X}{:02X}-"
					"{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
					data[0], data[1], data[2], data[3],
					data[4], data[5],
					data[6], data[7],
					data[8], data[9],
					data[10], data[11], data[12], data[13], data[14], data[15]
				),
				a_Context
			);
		}
	};

} // namespace std