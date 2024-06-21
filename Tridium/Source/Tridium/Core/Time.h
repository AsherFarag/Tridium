#pragma once

namespace Tridium {

	class Timestep
	{
	public:
		Timestep( float time = 0.0f )
			: m_Time( time ) {}

		operator float() const { return (float)m_Time; }
		operator double() const { return m_Time; }

		double GetSeconds() const { return m_Time; }
		double GetMilliseconds() const { return m_Time * 1000.0f; }

	private:
		double m_Time;
	};

	class Time
	{
		friend class Application;
	public:
		static const double& Get() { return s_Time; } // Returns the time since this application was first executed.
		static const double DeltaTime() { return s_DeltaTime; } // Returns the time it took to do the previous frame.
		static const Timestep GetTimestep() { return s_TimeStep; }
		static const void SetTimestep(Timestep a_NewTimestep ) { s_TimeStep = a_NewTimestep; }

	private:
		static void Update();

	private:
		static double s_Time;
		static double s_DeltaTime;
		static Timestep s_TimeStep;
	};

}