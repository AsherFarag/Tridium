#include "tripch.h"
#include "TimeStamp.h"
#include <ctime>

namespace Tridium {

	TimeStamp TimeStamp::Now()
	{
		return TimeStamp( Cast<::Tridium::time_t>( std::time( nullptr ) ) );
	}

	void TimeStamp::ToString( char* a_Buffer, int a_BufferSize, const char* a_Format ) const
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		std::strftime( a_Buffer, a_BufferSize, a_Format, tm);
	}

	String TimeStamp::ToString( const char* a_Format ) const
	{
		char buffer[64] = { 0 };
		ToString( buffer, 64, a_Format);
		return buffer;
	}

	uint32_t TimeStamp::Year() const
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		return Cast<uint32_t>( tm->tm_year + 1900 ); // tm_year is years since 1900
	}

	uint32_t TimeStamp::Month() const
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		return Cast<uint32_t>( tm->tm_mon + 1 ); // tm_mon is 0-11
	}

	uint32_t TimeStamp::Day() const
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		return Cast<uint32_t>( tm->tm_mday ); // tm_mday is 1-31
	}

	uint32_t TimeStamp::Hour() const
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		return Cast<uint32_t>( tm->tm_hour ); // tm_hour is 0-23
	}

	uint32_t TimeStamp::Minute() const
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		return Cast<uint32_t>( tm->tm_min ); // tm_min is 0-59
	}

	uint32_t TimeStamp::Second() const
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		return Cast<uint32_t>( tm->tm_sec ); // tm_sec is 0-60 (60 for leap seconds)
	}

	void TimeStamp::SetYear( uint32_t a_Year )
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		tm->tm_year = Cast<int>( a_Year - 1900 ); // tm_year is years since 1900
		Time = Cast<time_t>( std::mktime( tm ) );
	}

	void TimeStamp::SetMonth( uint32_t a_Month )
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		tm->tm_mon = Cast<int>( a_Month - 1 ); // tm_mon is 0-11
		Time = Cast<time_t>( std::mktime( tm ) );
	}

	void TimeStamp::SetDay( uint32_t a_Day )
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		tm->tm_mday = Cast<int>( a_Day ); // tm_mday is 1-31
		Time = Cast<time_t>( std::mktime( tm ) );
	}

	void TimeStamp::SetHour( uint32_t a_Hour )
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		tm->tm_hour = Cast<int>( a_Hour ); // tm_hour is 0-23
		Time = Cast<time_t>( std::mktime( tm ) );
	}

	void TimeStamp::SetMinute( uint32_t a_Minute )
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		tm->tm_min = Cast<int>( a_Minute ); // tm_min is 0-59
		Time = Cast<time_t>( std::mktime( tm ) );
	}

	void TimeStamp::SetSecond( uint32_t a_Second )
	{
		std::tm* tm = std::localtime( ReinterpretCast<const ::time_t*>( &Time ) );
		tm->tm_sec = Cast<int>( a_Second ); // tm_sec is 0-60 (60 for leap seconds)
		Time = Cast<time_t>( std::mktime( tm ) );
	}
}