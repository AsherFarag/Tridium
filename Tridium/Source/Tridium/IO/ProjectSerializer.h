#pragma once
#include <Tridium/Project/ProjectConfig.h>

namespace Tridium {

	class ProjectSerializer
	{
	public:
		static void SerializeText( const OldProjectConfig& a_Project, const FilePath& a_Path );
		static bool DeserializeText( OldProjectConfig& o_Project, const FilePath& a_Path );
	};

}