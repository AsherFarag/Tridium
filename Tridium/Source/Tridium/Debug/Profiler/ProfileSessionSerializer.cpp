#include "tripch.h"
#include "ProfileSessionSerializer.h"
#include <fstream>

namespace Tridium {

    bool ProfileSessionSerializer::SerializeJSON( const ProfilerSession& a_Session, const String& a_Path )
    {
        std::ofstream file( a_Path );
		if ( !ASSERT( file.is_open(), "Failed to open file for writing." ) )
		{
			return false;
		}

		// Convert from nanoseconds to micro
		auto ToMicro = []( TimeStamp::TimeType a_Time )->TimeStamp::TimeType
			{
				return a_Time / 1000;
			};

        file << "[\n"; // Start of JSON array
        for ( const auto& [threadID, thread] : a_Session.ProfiledThreads )
        {
			for ( const auto& frame : thread.Frames )
            {
                for ( const auto& result : frame.CollectedResults )
                {
                    // Begin Event
                    file << "  {\n"
                        << "    \"name\": \"" << result.Description->Name << "\",\n"
                        << "    \"cat\": \"" << result.Description->Filter.Name << "\",\n"
                        << "    \"ph\": \"B\",\n"
                        << "    \"ts\": " << ToMicro( result.TimeStamp.Start ) << ",\n"
                        << "    \"pid\": 0,\n"
                        << "    \"tid\": " << result.ThreadID << "\n"
                        << "  },\n";

                    // End Event
                    file << "  {\n"
                        << "    \"name\": \"" << result.Description->Name << "\",\n"
                        << "    \"cat\": \"" << result.Description->Filter.Name << "\",\n"
                        << "    \"ph\": \"E\",\n"
                        << "    \"ts\": " << ToMicro( result.TimeStamp.End ) << ",\n"
                        << "    \"pid\": 0,\n"
                        << "    \"tid\": " << result.ThreadID << "\n"
                        << "  }";
                }
            }
        }

        file << "\n]\n"; // End of JSON array
        file.close();

		LOG( LogCategory::Serialization, Info, "Profile session serialized to '{0}'", a_Path );
		return true;
    }

}
