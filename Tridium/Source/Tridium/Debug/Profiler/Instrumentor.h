#pragma once
#include <thread>
#include <Tridium/Containers/Stack.h>
#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/Map.h>
#include <Tridium/Containers/Tuple.h>
#include "ProfilerTask.h"
#include <Tridium/Utils/Singleton.h>
#include <Tridium/Core/Memory.h>

namespace Tridium {

	struct ProfilerSession
	{
		struct ThreadInfo
		{
			String Name;
			std::thread::id ID;
		};

		struct FrameData
		{
			Stack<ProfileResult> CallStack;
			Array<ProfileResult> CollectedResults;
			TimeStamp TimeStamp;
		};

		struct ProfiledThreadResult
		{
			ThreadInfo Info;
			Array<FrameData> Frames;
		};

		String Name;
		size_t NumOfFrames;
		TimeStamp TimeStamp;
		UnorderedMap<std::thread::id, ProfiledThreadResult> ProfiledThreads;
	};

	TODO( "Make this thread safe" );
	class Instrumentor final : public ISingleton<Instrumentor, /* ExplicitSetup */ false>
	{
	public:
		Instrumentor();
		virtual ~Instrumentor();

		enum class EState
		{
			Idle,
			Recording
		};

		void BeginSession( const String& a_Name, size_t a_NumOfFrames = 0 );
		void EndSession();
		SharedPtr<ProfilerSession> GetRecordedSession() const { return IsRecording() ? nullptr : m_CurrentSession; }

		void BeginFrame();
		void EndFrame();

		void BeginScope( const ProfileDescription* a_Description );
		void EndScope();

		bool IsRecording() const { return m_CurrentState == EState::Recording && m_CurrentSession != nullptr; }

		TimeStamp::TimeType GetTime() const;
		EState CurrentState() const { return m_CurrentState; }
		EState NextState() const { return m_NextState; }

	private:
		auto& GetCurrentThreadData() { return m_CurrentSession->ProfiledThreads[std::this_thread::get_id()]; }
		auto& GetCurrentFramaData() { return GetCurrentThreadData().Frames.Back(); }

		void StopRecording() { m_NextState = EState::Idle; }
		void StartRecording() { m_NextState = EState::Recording; }

	private:
		SharedPtr<ProfilerSession> m_CurrentSession = nullptr;
		size_t m_SessionFrameCap = 0;
		EState m_CurrentState = EState::Idle;
		EState m_NextState = EState::Idle;

		bool m_IsFrameComplete = false;
	};

	class ProfileScopeGuard
	{
	public:
		ProfileScopeGuard( const ProfileDescription* a_Description )
		{
			Instrumentor::Get()->BeginScope( a_Description );
		}

		~ProfileScopeGuard()
		{
			Instrumentor::Get()->EndScope();
		};

		ProfileScopeGuard( const ProfileScopeGuard& ) = delete;
		ProfileScopeGuard& operator=( const ProfileScopeGuard& ) = delete;
	};

	class ProfileFrameGuard
	{
	public:
		ProfileFrameGuard()
		{
			Instrumentor::Get()->BeginFrame();
		}
		~ProfileFrameGuard()
		{
			Instrumentor::Get()->EndFrame();
		};
		ProfileFrameGuard( const ProfileFrameGuard& ) = delete;
		ProfileFrameGuard& operator=( const ProfileFrameGuard& ) = delete;
	};

} // namespace Tridium