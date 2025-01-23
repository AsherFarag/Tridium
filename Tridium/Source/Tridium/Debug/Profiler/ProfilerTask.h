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
		const char* Name;
		const char* File;
		uint32_t Line;
		ProfilerFilter::Filter Filter;
		ProfilerColor Color;

		constexpr ProfileDescription( const char* a_Name, const char* a_File, uint32_t a_Line, ProfilerFilter::Filter a_Filter, ProfilerColor a_Color )
			: Name( a_Name ), File( a_File ), Line( a_Line ), Filter( a_Filter ), Color( a_Color )
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
		TimeType Start; // In Nanoseconds.
		TimeType End; // In Nanoseconds.

		TimeType GetDuration() const { return End - Start; }
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
	};

}