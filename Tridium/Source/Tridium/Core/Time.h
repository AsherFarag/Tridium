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
		static const double DeltaTime() { return s_DeltaTime; }
		static const Timestep GetTimestep() { return s_TimeStep; }

	private:
		static double s_DeltaTime;
		static Timestep s_TimeStep;
	};

}