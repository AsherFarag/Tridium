#pragma once
#include "ProfilerCategory.h"
#include <Tridium/Core/Containers/String.h>

namespace Tridium {

	// ========================================================================
	// Profile Description
	//  A struct to store data for a profiled scope, such as the function name.
	//  Instances of this class are static and created at compile time.
	// ========================================================================
	struct ProfileDescription
	{
		const char* Name; // Name of the profile.
		const char* Signature; // Optional signature of the profile. E.g. "void Function(int, float)"
		const char* File;
		uint32_t Line;
		ProfilerFilter::Filter Filter;
		ProfilerColor Color;
		size_t ID;

		constexpr ProfileDescription( const char* a_Name, const char* a_File, uint32_t a_Line, ProfilerFilter::Filter a_Filter, ProfilerColor a_Color )
			: ProfileDescription( a_Name, nullptr, a_File, a_Line, a_Filter, a_Color )
		{
		}

		constexpr ProfileDescription( const char* a_Name, const char* a_Signature, const char* a_File, uint32_t a_Line, ProfilerFilter::Filter a_Filter, ProfilerColor a_Color )
			: Name( a_Name )
			, Signature(a_Signature)
			, File( a_File )
			, Line( a_Line )
			, Filter( a_Filter )
			, Color( a_Color )
			, ID( Hashing::HashCombine( Hashing::HashString( a_File ), static_cast<size_t>(a_Line) ) )
		{
		}
	};

	// ========================================================================
	// Time Stamp
	//  A struct to store the start and end time of a profiled scope.
	// ========================================================================
	struct TimeStamp
	{
		using TimeType = int64_t;
		static constexpr TimeType s_InvalidTimeStamp = -1;
		TimeType Start; // In Microseconds.
		TimeType End; // In Microseconds.

		TimeType GetDuration() const { return End - Start; }
		double GetDurationMilli() const { return static_cast<double>( GetDuration() ) / 1000.0; }
		double GetDurationSeconds() const { return static_cast<double>( GetDuration() ) / 1000000.0; }
		double GetStartMilli() const { return static_cast<double>( Start ) / 1000.0; }
		double GetStartSeconds() const { return static_cast<double>( Start ) / 1000000.0; }
		double GetEndMilli() const { return static_cast<double>( End ) / 1000.0; }
		double GetEndSeconds() const { return static_cast<double>( End ) / 1000000.0; }

		bool operator==( const TimeStamp& other ) const
		{
			return Start == other.Start && End == other.End;
		}
	};

	// ========================================================================
	// Profile Result
	//  A struct to store the result of a profiled scope.
	// ========================================================================
	struct ProfileResult
	{
		const ProfileDescription* Description = nullptr; // The description of the profiled scope.
		uint32_t ThreadID = 0u; // The ID of the thread that the scope was profiled on.
		uint32_t Depth = 0u; // The depth of the call stack.
		TimeStamp TimeStamp; // The time stamp of the profiled scope.

		bool operator<( const ProfileResult& other ) const
		{
			return TimeStamp.Start < other.TimeStamp.Start;
		}

		bool operator==( const ProfileResult& other ) const
		{
			return Description == other.Description 
				&& TimeStamp == other.TimeStamp
				&& ThreadID == other.ThreadID
				&& Depth == other.Depth;
		}

		bool operator!=( const ProfileResult& other ) const
		{
			return !( *this == other );
		}
	};

}