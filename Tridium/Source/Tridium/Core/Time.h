#pragma once

namespace Tridium {

	class Time
	{
		friend class Application;
	public:
		static const double DeltaTime() { return s_DeltaTime; }

	private:
		static double s_DeltaTime;
	};

}