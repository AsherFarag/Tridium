#pragma once
#include <chrono>

namespace Tridium {

	template<typename T, typename _TimeUnit = std::chrono::milliseconds>
	class ScopedTimer
	{
	public:
		using Clock = std::chrono::high_resolution_clock;

		ScopedTimer( T& a_Timer )
			: m_Timer( a_Timer )
		{
			m_Start = Clock::now();
		}

		~ScopedTimer()
		{
			auto end = Clock::now();
			auto duration = std::chrono::duration_cast<_TimeUnit>( ( end - m_Start ) * 1000.0 );
			m_Timer = duration.count() / 1000.0;
		}

	private:
		T& m_Timer;
		std::chrono::time_point<Clock> m_Start;
	};
}