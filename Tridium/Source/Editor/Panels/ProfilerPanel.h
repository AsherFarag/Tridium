#pragma once
#include "Panel.h"
#include <Tridium/Core/Containers/Optional.h>
#include <Tridium/Core/Delegate.h>

namespace Tridium::Editor {

	class ProfilerPanel : public Panel
	{
		struct TimelineData
		{
			ImRect Rect;
			double TimeToPixels = 1.0f;

			struct Range
			{
				double Start = 0.0f;
				double End = 0.0f;

				double GetWidth() const { return End - Start; }
			};
			Range SessionTimeRange;
			Range VisibleTimeRange;
		};

	public:
		ProfilerPanel();
		virtual ~ProfilerPanel();

		virtual void OnImGuiDraw() override;

	private:
		void DrawSession( ProfilerSession& a_Session );
		void DrawThread( size_t a_ThreadID, const ProfilerSession::ProfiledThreadResult& a_ThreadData, TimelineData a_TimelineData );
		void RetrieveRecordedSession();

	private:
		SharedPtr<ProfilerSession> m_Session;

		struct TimelineSettings
		{
			struct Range
			{
				float Min;
				float Max;
			};

			float Height = 100.0f;
			Range ZoomRange = { 0.1f, 1000.0f };
			float ZoomFactor = 1.0f;
			int64_t PanOffset = 0.0f;
			uint32_t MaxDepth = 0u;
			float FontSize = 1;

			enum ENameDisplayMode
			{
				None,
				Name,
				Signature,
				Filter,
			} NameDisplayMode = ENameDisplayMode::Name;
		} m_TimelineSettings;

		Optional<ProfileResult> m_SelectedResult;
		DelegateHandle m_OnEndFrameDelegateHandle;
	};

}