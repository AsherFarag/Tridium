#include "tripch.h"
#include "Time.h"
#include "GLFW/glfw3.h"

namespace Tridium {

	double Time::s_Time = 0.0;
	double Time::s_DeltaTime = 0.0;
	Timestep Time::s_TimeStep = 1.0 / 60.0;

	void Time::Update()
	{
		s_Time = glfwGetTime();
	}

} // namespace Tridium