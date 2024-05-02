#pragma once

#include "Core.h"

namespace Tridium
{
	class TRIDIUM_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

