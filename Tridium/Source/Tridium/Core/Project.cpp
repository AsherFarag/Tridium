#include "tripch.h"
#include "Project.h"

namespace Tridium {
	Ref<Project> Project::s_ActiveProject = MakeRef<Project>();
}