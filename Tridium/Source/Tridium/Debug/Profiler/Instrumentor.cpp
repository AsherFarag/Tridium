#include "tripch.h"
#include "Instrumentor.h"
#include <chrono>
#include <Tridium/Core/Application.h>

namespace Tridium {

	Instrumentor::Instrumentor()
	{
	}

	Instrumentor::~Instrumentor()
	{
	}

	void Instrumentor::BeginSession( const String& a_Name, size_t a_NumOfFrames )
	{
		m_SessionFrameCap = a_NumOfFrames;
		m_NextState = EState::Recording;

		m_CurrentSession = MakeShared<ProfilerSession>();
		m_CurrentSession->Name = a_Name;
		m_CurrentSession->TimeStamp = { GetTime(), TimeStamp::s_InvalidTimeStamp };

		TODO("Workaround for threads not being created automatically");
		m_CurrentSession->ProfiledThreads[std::this_thread::get_id()];
	}

	void Instrumentor::EndSession()
	{
		StopRecording();
		m_CurrentSession->TimeStamp.End = GetTime();

		// If the frame is not complete, remove it
		if ( !m_IsFrameComplete )
		{
			for ( auto& [threadID, threadData] : m_CurrentSession->ProfiledThreads )
			{
				threadData.Frames.PopBack();
			}

			m_CurrentSession->NumOfFrames--;
		}
	}

	void Instrumentor::BeginFrame()
	{
		m_CurrentState = m_NextState;
		if ( !IsRecording() )
			return;

		if ( m_SessionFrameCap > 0 && m_CurrentSession->NumOfFrames >= m_SessionFrameCap )
		{
			EndSession();
			return;
		}

		m_IsFrameComplete = false;
		m_CurrentSession->NumOfFrames++;

		TimeStamp::TimeType currentTime = GetTime();

		for ( auto& [threadID, threadData] : m_CurrentSession->ProfiledThreads )
		{
			auto& frameData = threadData.Frames.EmplaceBack();
			frameData.TimeStamp = { currentTime, TimeStamp::s_InvalidTimeStamp };
		}
	}

	void Instrumentor::EndFrame()
	{
		if ( !IsRecording() )
			return;

		m_IsFrameComplete = true;

		TimeStamp::TimeType currentTime = GetTime();
		for ( auto& [threadID, threadData] : m_CurrentSession->ProfiledThreads )
		{
			auto& frameData = threadData.Frames.Back();
			frameData.TimeStamp.End = currentTime;
		}
	}

	void Instrumentor::BeginScope( const ProfileDescription* a_Description )
	{
		if ( !IsRecording() )
			return;

		auto& frameData = GetCurrentFramaData();

		ProfileResult result =
		{
			.Description = a_Description,
			.ThreadID = Cast<uint32_t>( std::hash<std::thread::id>{}( std::this_thread::get_id() ) ),
			.TimeStamp = { GetTime(), TimeStamp::s_InvalidTimeStamp }
		};

		frameData.CallStack.emplace( std::move( result ) );
	}

	void Instrumentor::EndScope()
	{
		if ( !IsRecording() )
			return;

		auto& frameData = GetCurrentFramaData();
		if ( frameData.CallStack.empty() )
			return;

		ProfileResult result = frameData.CallStack.top();
		frameData.CallStack.pop();
		result.TimeStamp.End = GetTime();
		result.Depth = Cast<uint32_t>( frameData.CallStack.size() );
		frameData.CollectedResults.PushBack( result );
	}

	TimeStamp::TimeType Instrumentor::GetTime() const
	{
		long long nowMicroSeconds = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now().time_since_epoch() ).count();
		return Cast<TimeStamp::TimeType>( nowMicroSeconds );
	}

} // namespace Tridium