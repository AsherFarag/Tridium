#pragma once
#include <Tridium/Core/Types.h>

namespace Tridium {

	using time_t = uint64_t;

	struct TimeStamp
	{
		static constexpr const char* c_DefaultFormat = "%Y-%m-%d %H:%M:%S";
		time_t Time = 0;

		static TimeStamp Now();
		void ToString( char* a_Buffer, int a_BufferSize, const char* a_Format = c_DefaultFormat ) const;
		String ToString( const char* a_Format = c_DefaultFormat ) const;

		bool Valid() const noexcept { return Time != 0; }

		uint32_t Year() const;
		uint32_t Month() const;
		uint32_t Day() const;
		uint32_t Hour() const;
		uint32_t Minute() const;
		uint32_t Second() const;

		void SetYear( uint32_t a_Year );
		void SetMonth( uint32_t a_Month );
		void SetDay( uint32_t a_Day );
		void SetHour( uint32_t a_Hour );
		void SetMinute( uint32_t a_Minute );
		void SetSecond( uint32_t a_Second );

		constexpr auto operator<=>( const TimeStamp& ) const noexcept = default;
		constexpr bool operator==( const TimeStamp& ) const noexcept = default;

		constexpr auto operator+( const TimeStamp& a_Other ) const noexcept { return TimeStamp{ Time + a_Other.Time }; }
		constexpr auto operator-( const TimeStamp& a_Other ) const noexcept { return TimeStamp{ Time - a_Other.Time }; }
		constexpr auto& operator+=( const TimeStamp& a_Other ) noexcept { Time += a_Other.Time; return *this; }
		constexpr auto& operator-=( const TimeStamp& a_Other ) noexcept { Time -= a_Other.Time; return *this; }
	};

}