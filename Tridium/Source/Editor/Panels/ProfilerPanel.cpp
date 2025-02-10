#include "tripch.h"
#include "ProfilerPanel.h"
#include <Tridium/ImGui/IconsFontAwesome6.h>
#include <Tridium/Core/Application.h>
#include <Editor/EditorStyle.h>

namespace Tridium {

	ProfilerPanel::ProfilerPanel()
		: Panel( "Profiler" )
	{
	}

	ProfilerPanel::~ProfilerPanel()
	{
	}

	void ProfilerPanel::OnImGuiDraw()
	{
		if ( ImGui::Begin( m_Name.c_str(), &m_Open ) )
		{
			const bool isRecording = Instrumentor::Get()->IsRecording();

			{
				ImGui::BeginDisabled( isRecording );
				if ( ImGui::ButtonEx( "Start Recording" ) )
				{
					if ( !Instrumentor::Get()->IsRecording() )
					{
						Instrumentor::Get()->BeginSession( "Profiler" );
					}
				}
				ImGui::EndDisabled();
			}

			ImGui::SameLine();

			{
				ImGui::BeginDisabled( !isRecording );
				if ( ImGui::Button( "Stop Recording" ) )
				{
					if ( Instrumentor::Get()->IsRecording() )
					{
						Instrumentor::Get()->EndSession();
					}
				}
				ImGui::EndDisabled();
			}

			ImGui::SameLine();

			if ( isRecording )
				ImGui::Text( "Recording..." );
			else
				ImGui::Text( "Not Recording" );

			{
				float offsetFromLeft = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize( TE_ICON_GEAR ).x - ImGui::GetStyle().FramePadding.x * 2.0f;
				ImGui::SameLine( offsetFromLeft );

				if ( ImGui::SmallButton( TE_ICON_GEAR ) )
					ImGui::OpenPopup( "Options" );

				if ( ImGui::BeginPopup( "Options" ) )
				{
					ImGui::DragFloat( "Timeline Height", &m_TimelineSettings.Height, 1.0f, 50.0f, 500.0f );
					ImGui::DragFloat( "Zoom Factor", &m_TimelineSettings.ZoomFactor, 0.1f, m_TimelineSettings.ZoomRange.Min, m_TimelineSettings.ZoomRange.Max );
					ImGui::DragScalar( "Pan Offset", ImGuiDataType_S64, &m_TimelineSettings.PanOffset, 10000.0, nullptr, nullptr, "%lld" );
					ImGui::DragFloat( "Font Size", &m_TimelineSettings.FontSize, 0.01f, 0.001f, 2.0f );

					// Option Menu for name display mode
					constexpr const char* nameDisplayModes[] = { "None", "Name", "Signature", "Filter" };
					const char* previewNameDisplayMode = nameDisplayModes[m_TimelineSettings.NameDisplayMode];
					if ( ImGui::BeginCombo( "Name Display Mode", previewNameDisplayMode ) )
					{
						if ( ImGui::Selectable( "None", m_TimelineSettings.NameDisplayMode == TimelineSettings::ENameDisplayMode::None ) )
							m_TimelineSettings.NameDisplayMode = TimelineSettings::ENameDisplayMode::None;
						if ( ImGui::Selectable( "Name", m_TimelineSettings.NameDisplayMode == TimelineSettings::ENameDisplayMode::Name ) )
							m_TimelineSettings.NameDisplayMode = TimelineSettings::ENameDisplayMode::Name;
						if ( ImGui::Selectable( "Signature", m_TimelineSettings.NameDisplayMode == TimelineSettings::ENameDisplayMode::Signature ) )
							m_TimelineSettings.NameDisplayMode = TimelineSettings::ENameDisplayMode::Signature;
						if ( ImGui::Selectable( "Filter", m_TimelineSettings.NameDisplayMode == TimelineSettings::ENameDisplayMode::Filter ) )
							m_TimelineSettings.NameDisplayMode = TimelineSettings::ENameDisplayMode::Filter;
						ImGui::EndCombo();
					}

					if ( ImGui::Button( "Reset" ) )
					{
						m_Session.reset();
					}

					ImGui::EndPopup();
				}
			}

			ImGui::Separator();

			// Draw the current session
			RetrieveRecordedSession();
			if ( m_Session )
			{
				DrawSession( *m_Session );
			}
		}

		ImGui::End();
	}

	void ProfilerPanel::DrawSession( ProfilerSession& a_Session )
	{
		float timeToPixels = 1.0f;

		// Create a table with a row for each thread and a column splitting the thread name and timeline
		const ImGuiTableFlags tableFlags =
			ImGuiTableFlags_BordersInner |
			ImGuiTableFlags_Resizable |
			ImGuiTableFlags_BordersOuter;
		if ( ImGui::BeginTable( "##ThreadTable", 2, tableFlags ) )
		{
			ImGui::TableSetupColumn( "Thread", ImGuiTableColumnFlags_WidthFixed, 200 );
			ImGui::TableSetupColumn( "Timeline", ImGuiTableColumnFlags_WidthStretch );
			ImGui::TableHeadersRow();

			for ( const auto& [threadID, thread] : a_Session.ProfiledThreads )
			{
				const uint32_t id = reinterpret_cast<const uint32_t&>( threadID );
				ImGui::TableNextRow();

				// Draw the thread name and id
				ImGui::TableSetColumnIndex( 0 );
				ImGui::Text( "Name: %s", thread.Info.Name.empty() ? "Unknown" : thread.Info.Name.c_str() );

				ImGui::Text( "Thread ID: %lu", thread.Info.Name.c_str(), id );

				// Draw the thread timeline
				ImGui::TableSetColumnIndex( 1 );
				TimelineData timelineData;
				timelineData.Rect.Min = ImGui::GetCursorScreenPos();
				timelineData.Rect.Max = timelineData.Rect.Min + ImVec2( ImGui::GetContentRegionAvail().x, m_TimelineSettings.Height );
				timelineData.SessionTimeRange.Start = a_Session.TimeStamp.Start;
				timelineData.SessionTimeRange.End = a_Session.TimeStamp.End;
				timelineData.VisibleTimeRange.Start = timelineData.SessionTimeRange.Start + m_TimelineSettings.PanOffset;
				timelineData.VisibleTimeRange.End = timelineData.VisibleTimeRange.Start + ( timelineData.SessionTimeRange.GetWidth() / m_TimelineSettings.ZoomFactor );
				timelineData.TimeToPixels = timelineData.Rect.GetWidth() / timelineData.VisibleTimeRange.GetWidth();

				timeToPixels = timelineData.TimeToPixels;

				DrawThread( id, thread, timelineData );
			}

			ImGui::EndTable();
		}

		ImGui::Separator();
		// Draw selected result info
		if ( m_SelectedResult )
		{
			const ImGuiTableFlags tableFlags =
				ImGuiTableFlags_BordersInner |
				ImGuiTableFlags_Resizable |
				ImGuiTableFlags_BordersOuter;
			if ( ImGui::BeginTable( "##SelectedResultTable", 2, tableFlags ) )
			{
				ImGui::TableSetupColumn( "Variable", ImGuiTableColumnFlags_WidthFixed, 200 );
				ImGui::TableSetupColumn( "Value", ImGuiTableColumnFlags_WidthStretch );

				// Name
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::TextUnformatted( "Name" );
					ImGui::TableSetColumnIndex( 1 );
					ImGui::TextUnformatted( m_SelectedResult->Description->Name );
				}

				// Filter
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::TextUnformatted( "Filter" );
					ImGui::TableSetColumnIndex( 1 );
					ImGui::TextUnformatted( m_SelectedResult->Description->Filter.Name );

					// Draw the filter color as a square
					const ImVec2 squareSize = { ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() };
					const ImU32 color = m_SelectedResult->Description->Color;
					ImGui::SameLine();
					ImGui::GetWindowDrawList()->AddRectFilled( ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + squareSize, color );

				}

				// Signature
				if ( m_SelectedResult->Description->Signature )
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::TextUnformatted( "Signature" );
					ImGui::TableSetColumnIndex( 1 );
					ImGui::TextUnformatted( m_SelectedResult->Description->Signature );
				}

				// Duration
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::TextUnformatted( "Duration" );
					ImGui::TableSetColumnIndex( 1 );
					ImGui::Text( "%.3fms", m_SelectedResult->TimeStamp.GetDurationMilli() );
				}

				// Start
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::TextUnformatted( "Start" );
					ImGui::TableSetColumnIndex( 1 );
					ImGui::Text( "%.3f seconds", m_SelectedResult->TimeStamp.GetStartSeconds() - a_Session.TimeStamp.GetStartSeconds() );
				}

				// End
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::TextUnformatted( "End" );
					ImGui::TableSetColumnIndex( 1 );
					ImGui::Text( "%.3f seconds", m_SelectedResult->TimeStamp.GetEndSeconds() - a_Session.TimeStamp.GetStartSeconds() );
				}

				// Depth
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::TextUnformatted( "Depth" );
					ImGui::TableSetColumnIndex( 1 );
					ImGui::Text( "%d", m_SelectedResult->Depth );
				}

				// File
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::TextUnformatted( "File" );
					ImGui::TableSetColumnIndex( 1 );
					ImGui::TextUnformatted( m_SelectedResult->Description->File );
				}

				// Line
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex( 0 );
					ImGui::TextUnformatted( "Line" );
					ImGui::TableSetColumnIndex( 1 );
					ImGui::Text( "%d", m_SelectedResult->Description->Line );
				}

				ImGui::EndTable();
			}
		}

		// Handle input for zooming and panning
		ImVec2 winMin = ImGui::GetWindowPos();
		ImVec2 winMax = winMin + ImGui::GetWindowSize();
		if ( ImGui::IsMouseHoveringRect( winMin, winMax ) )
		{
			const float scroll = ImGui::GetIO().MouseWheel;
			float scrollSpeed = 1.0f;
			if ( ImGui::GetIO().KeyShift )
				scrollSpeed *= 10.0f;
			if ( ImGui::GetIO().KeyCtrl )
				scrollSpeed *= 0.05f;

			if ( scroll != 0.0f )
			{
				m_TimelineSettings.ZoomFactor = Math::Clamp( 
					m_TimelineSettings.ZoomFactor + ImGui::GetIO().MouseWheel * scrollSpeed,
					m_TimelineSettings.ZoomRange.Min,
					m_TimelineSettings.ZoomRange.Max );
			}

			if ( ImGui::IsMouseDragging( ImGuiMouseButton_Right ) )
			{
				ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeAll );
				m_TimelineSettings.PanOffset -= (int64_t)( ImGui::GetIO().MouseDelta.x / timeToPixels );
			}
			else
			{
				ImGui::SetMouseCursor( ImGuiMouseCursor_Arrow );
			}
		}
	}

	void ProfilerPanel::DrawThread( size_t a_ThreadID, const ProfilerSession::ProfiledThreadResult& a_ThreadData, TimelineData a_TimelineData )
	{
		ImGui::SetWindowFontScale( m_TimelineSettings.FontSize );

		const float height = a_TimelineData.Rect.GetHeight() / ( m_TimelineSettings.MaxDepth + 1 );
		const ImU32 textColor = IM_COL32_BLACK;
		const ImVec2 ellipsisSize = ImGui::CalcTextSize( "..." );

		if ( ImGui::BeginChild( a_ThreadID, a_TimelineData.Rect.GetSize(), ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollbar ) )
		{
			// Draw the timeline background
			ImGui::GetWindowDrawList()->AddRectFilled( a_TimelineData.Rect.Min, a_TimelineData.Rect.Max, IM_COL32( 50, 50, 55, 200 ) );

			// Draw the event timeline for each profile result
			int index = -1;
			for ( const auto& frame : a_ThreadData.Frames )
			{
				for ( const auto& result : frame.CollectedResults )
				{
					index++;
					double start = result.TimeStamp.Start;
					double end = result.TimeStamp.End;

					// Skip results that are not visible
					if ( start >= a_TimelineData.VisibleTimeRange.End || end <= a_TimelineData.VisibleTimeRange.Start )
						continue;

					// Convert time to pixel space
					start = ( start - a_TimelineData.VisibleTimeRange.Start ) * a_TimelineData.TimeToPixels;
					end = ( end - a_TimelineData.VisibleTimeRange.Start ) * a_TimelineData.TimeToPixels;

					// Calculate the rectangle for the result
					ImRect resultRect = {
						ImVec2( a_TimelineData.Rect.Min.x + start, a_TimelineData.Rect.Min.y + ( result.Depth * height ) ),
						ImVec2( a_TimelineData.Rect.Min.x + end, a_TimelineData.Rect.Min.y + ( ( result.Depth + 1 ) * height ) )
					};

					// Determine text to display based on name display mode
					const char* textToDisplay = nullptr;
					switch ( m_TimelineSettings.NameDisplayMode )
					{
						using enum TimelineSettings::ENameDisplayMode;
					case None:
						textToDisplay = "";
						break;
					case Name:
						textToDisplay = result.Description->Name;
						break;
					case Signature:
						textToDisplay = result.Description->Signature ? result.Description->Signature : result.Description->Name;
						break;
					case Filter:
						textToDisplay = result.Description->Filter.Name;
						break;
					}

					// Draw the event rectangle
					{
						ImGui::SetCursorScreenPos( resultRect.Min );

						// Set hovered color to be lighter
						// This is a bit of a hack to get the hovered color to be lighter
						union Col32
						{
							ImU32 Value;
							struct { ImU8 r, g, b, a; };
						};
						Col32 hoveredColor = { result.Description->Color };
						hoveredColor.r = Math::Min( hoveredColor.r + 50u, 255u );
						hoveredColor.g = Math::Min( hoveredColor.g + 50u, 255u );
						hoveredColor.b = Math::Min( hoveredColor.b + 50u, 255u );

						ImGui::PushStyleColor( ImGuiCol_Button, result.Description->Color );
						ImGui::PushStyleColor( ImGuiCol_ButtonHovered, hoveredColor.Value );
						ImGui::PushStyleColor( ImGuiCol_ButtonActive, IM_COL32_WHITE );
						ImGui::PushStyleColor( ImGuiCol_Text, textColor );
						ImGui::PushStyleColor( ImGuiCol_Border, IM_COL32_WHITE );
						ImGui::PushStyleColor( ImGuiCol_BorderShadow, 0 );

						ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
						ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1.0f );

						ImGui::PushID( index );
						if ( ImGui::ButtonEx( textToDisplay, resultRect.GetSize() ) )
						{
							m_SelectedResult = result;
						}
						ImGui::PopID();

						ImGui::PopStyleVar( 2 );
						ImGui::PopStyleColor( 6 );


						// If this result is selected, draw a border around it
						if ( m_SelectedResult && m_SelectedResult.value() == result )
						{
							ImGui::GetWindowDrawList()->AddRect( resultRect.Min, resultRect.Max, Style::Colors::Orange, 0.0f, 0, 3.0f );
						}


						if ( ImGui::BeginItemTooltip() )
						{
							const float durationMilli = result.TimeStamp.GetDurationMilli();
							const float timeStartSeconds = result.TimeStamp.GetStartSeconds() - m_Session->TimeStamp.GetStartSeconds();
							const float timeEndSeconds = result.TimeStamp.GetStartSeconds() - m_Session->TimeStamp.GetStartSeconds();

							ImGui::Text( "Name: %s", result.Description->Name );
							ImGui::Text( "Filter: %s", result.Description->Filter.Name );
							ImGui::Text( "Duration: %.3fms", durationMilli );
							ImGui::Text( "Start: %.3f seconds", timeStartSeconds );
							ImGui::Text( "End: %.3f seconds", timeStartSeconds );
							ImGui::Text( "Depth: %d", result.Depth );

							ImGui::EndTooltip();
						}
					}
				}
			}
		}

		//

		ImGui::EndChild();
		ImGui::SetWindowFontScale( 1.0f );
	}

	void ProfilerPanel::RetrieveRecordedSession()
	{
		m_Session = Instrumentor::Get()->GetRecordedSession();
		if ( !m_Session )
			return;

		// Get max depth
		for ( const auto& [threadID, thread] : m_Session->ProfiledThreads )
		{
			for ( const auto& frame : thread.Frames )
			{
				for ( const auto& result : frame.CollectedResults )
				{
					m_TimelineSettings.MaxDepth = Math::Max( m_TimelineSettings.MaxDepth, result.Depth );
				}
			}
		}
	}

} // namespace Tridium