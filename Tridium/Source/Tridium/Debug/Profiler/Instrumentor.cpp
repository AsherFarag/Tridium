#include "tripch.h"
#include "Instrumentor.h"
#include <chrono>

namespace Tridium {

	void Instrumentor::BeginSession( const String& a_Name )
	{
		if ( !ASSERT_LOG( m_CurrentSession == nullptr, "Attempting to begin a new profiler session without ending the current one." ) )
			return;

		m_CurrentSession = MakeUnique<ProfilerSession>();
		m_CurrentSession->Name = a_Name;
	}

	UniquePtr<ProfilerSession> Instrumentor::EndSession()
	{
		StopRecording();
		return std::move( m_CurrentSession );
	}

	void Instrumentor::BeginScope( const ProfileDescription* a_Description )
	{
		if ( !ASSERT_LOG( m_CurrentSession != nullptr, "Attempting to profile a scope without a session." ) )
			return;

		if ( !IsRecording() )
			return;

		ProfilerSession::ThreadData& threadData = GetCurrentThreadData();

		ProfileResult result =
		{
			.Description = a_Description,
			.ThreadID = static_cast<uint32_t>( std::hash<std::thread::id>{}( std::this_thread::get_id() ) ),
			.TimeStamp = { GetTime(), TimeStamp::s_InvalidTimeStamp }
		};

		threadData.CallStack.emplace( std::move( result ) );
	}

	void Instrumentor::EndScope()
	{
		if ( !ASSERT_LOG( m_CurrentSession != nullptr, "Attempting to end a profile scope without a session." ) )
			return;

		if ( !IsRecording() )
			return;

		ProfilerSession::ThreadData& threadData = GetCurrentThreadData();
		if ( !ASSERT_LOG( !threadData.CallStack.empty(), "Attempting to end a profiler scope without a matching begin scope." ) )
		{
			return;
		}

		ProfileResult result = threadData.CallStack.top();
		threadData.CallStack.pop();
		result.TimeStamp.End = GetTime();
		result.Depth = static_cast<uint32_t>( threadData.CallStack.size() );
		threadData.CollectedResults.PushBack( result );
	}

	TimeStamp::TimeType Instrumentor::GetTime() const
	{
		long long nowNanoSeconds = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		return static_cast<TimeStamp::TimeType>( nowNanoSeconds );
	}

}
