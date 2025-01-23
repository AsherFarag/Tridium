#pragma once
#include "Instrumentor.h"

namespace Tridium {

	class ProfileSessionSerializer
	{
	public:
		enum class EFormat
		{
			JSON,
			Binary
		};

		static bool Serialize( const ProfilerSession& a_Session, EFormat a_Format, const String& a_Path )
		{
			switch ( a_Format )
			{
			case EFormat::JSON:
				return SerializeJSON( a_Session, a_Path );
			case EFormat::Binary:
				return false;
			default:
				return false;
			}
		}

		// Serialize into Google's Trace Event Format
		static bool SerializeJSON( const ProfilerSession& a_Session, const String& a_Path );
	};

}