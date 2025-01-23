#pragma once
#include <thread>
#include <Tridium/Core/Containers/Stack.h>
#include <Tridium/Core/Containers/Array.h>
#include <Tridium/Core/Containers/Map.h>
#include "ProfilerTask.h"
#include <Tridium/Utils/Singleton.h>
#include <Tridium/Core/Memory.h>

namespace Tridium {

	struct ProfilerSession
	{
		struct ThreadData
		{
			Stack<ProfileResult> CallStack;
			Array<ProfileResult> CollectedResults;
		};

		String Name;
		TimeStamp TimeStamp;
		UnorderedMap<std::thread::id, ThreadData> ThreadDataMap;
	};

	TODO( "Make this thread safe" );
	class Instrumentor final : public ISingleton<Instrumentor, /* ExplicitSetup */ false>
	{
	public:
		enum class EState
		{
			Idle,
			Recording
		};

		void BeginSession( const String& a_Name );
		UniquePtr<ProfilerSession> EndSession();

		void BeginScope( const ProfileDescription* a_Description );
		void EndScope();

		bool IsRecording() const { return m_State == EState::Recording; }
		void StopRecording() { m_State = EState::Idle; }
		void StartRecording() { m_State = EState::Recording; }

		TimeStamp::TimeType GetTime() const;
		EState GetState() const { return m_State; }

	private:
		ProfilerSession::ThreadData& GetCurrentThreadData() { return m_CurrentSession->ThreadDataMap[std::this_thread::get_id()]; }

	private:
		UniquePtr<ProfilerSession> m_CurrentSession = nullptr;
		EState m_State = EState::Idle;
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

}