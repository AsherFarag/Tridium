#pragma once
#include <Tridium/Project/ProjectConfig.h>

namespace Tridium {

	class ProjectSerializer
	{
	public:
		static void SerializeText( const ProjectConfig& a_Project, const FilePath& a_Path );
		static bool DeserializeText( ProjectConfig& o_Project, const FilePath& a_Path );
	};

}