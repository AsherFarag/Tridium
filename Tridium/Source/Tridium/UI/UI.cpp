#include "tripch.h"
#include "UI.h"

#if CONFIG_ENABLE_TOOL_UI

namespace Util {

	template <typename T>
	inline T ImCubicBezier( const T& p0, const T& p1, const T& p2, const T& p3, float t )
	{
		const auto a = 1 - t;
		const auto b = a * a * a;
		const auto c = t * t * t;

		return b * p0 + 3 * t * a * a * p1 + 3 * t * t * a * p2 + c * p3;
	}

	// Project point on Cubic Bezier curve.
	struct ImProjectResult
	{
		ImVec2 Point;    // Point on curve
		float  Time;     // [0 - 1]
		float  Distance; // Distance to curve
	};

	static ImProjectResult ImProjectOnCubicBezier( const ImVec2& point, const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const int subdivisions = 100 )
	{
		// http://pomax.github.io/bezierinfo/#projections

		const float epsilon = 1e-5f;
		const float fixed_step = 1.0f / static_cast<float>( subdivisions - 1 );

		ImProjectResult result;
		result.Point = point;
		result.Time = 0.0f;
		result.Distance = FLT_MAX;

		// Step 1: Coarse check
		for ( int i = 0; i < subdivisions; ++i )
		{
			auto t = i * fixed_step;
			auto p = ImCubicBezier( p0, p1, p2, p3, t );
			auto s = point - p;
			auto d = ImDot( s, s );

			if ( d < result.Distance )
			{
				result.Point = p;
				result.Time = t;
				result.Distance = d;
			}
		}

		if ( result.Time == 0.0f || ImFabs( result.Time - 1.0f ) <= epsilon )
		{
			result.Distance = ImSqrt( result.Distance );
			return result;
		}

		// Step 2: Fine check
		auto left = result.Time - fixed_step;
		auto right = result.Time + fixed_step;
		auto step = fixed_step * 0.1f;

		for ( auto t = left; t < right + step; t += step )
		{
			auto p = ImCubicBezier( p0, p1, p2, p3, t );
			auto s = point - p;
			auto d = ImDot( s, s );

			if ( d < result.Distance )
			{
				result.Point = p;
				result.Time = t;
				result.Distance = d;
			}
		}

		result.Distance = ImSqrt( result.Distance );

		return result;
	}
}

namespace Tridium::UI {

	struct UIState
	{
		// Unique ID that is incremented and decremented with each PushID/PopID call.
		int UniqueIDStack = 0;
		int IDCounter = 0;

		// Used to track if a property grid is currently open, for proper ID management.
		int32_t PropertyGridStackCounter = 0;

		bool IsPropertyGridOpen() const
		{
			return PropertyGridStackCounter > 0;
		}
	};

	static UIState& GetUIState()
	{
		static UIState s_ToolUIState;
		return s_ToolUIState;
	}

	void ShiftCursor( float a_OffsetX, float a_OffsetY )
	{
		ImGui::SetCursorPos( ImGui::GetCursorPos() + ImVec2( a_OffsetX, a_OffsetY ) );
	}

	void PushID()
	{
		ImGui::PushID( GetUIState().UniqueIDStack++ );
		GetUIState().IDCounter = 0;
	}

	void PopID()
	{
		GetUIState().UniqueIDStack--;
		ASSERT( GetUIState().UniqueIDStack >= 0, "PopID called more times than PushID" );

		ImGui::PopID();
	}

	const char* GenerateID()
	{
		static thread_local char s_IDBuffer[16 + 2 + 1] = "##";
		snprintf( s_IDBuffer + 2, 16, "%u", GetUIState().IDCounter++ );
		return s_IDBuffer;
	}

	void BeginPropertyGrid( uint32_t a_NumColumns )
	{
		GetUIState().PropertyGridStackCounter++;

		PushID();
		ImGui::Columns( a_NumColumns );
	}

	void EndPropertyGrid()
	{
		ImGui::Columns( 1 );
		DrawUnderline();
		ShiftCursor( 0.0f, 18.0f );
		PopID();

		GetUIState().PropertyGridStackCounter--;

		ASSERT( GetUIState().PropertyGridStackCounter >= 0, "EndPropertyGrid called more times than BeginPropertyGrid" );
	}

	void DrawUnderline( bool a_SpanFullWidth, float a_OffsetX, float a_OffsetY )
	{
		if ( a_SpanFullWidth )
		{
			if ( ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr )
				ImGui::PushColumnsBackground();
			else if ( ImGui::GetCurrentTable() != nullptr )
				ImGui::TablePushBackgroundChannel();
		}

		const float width = a_SpanFullWidth ? ImGui::GetWindowWidth() : ImGui::GetContentRegionAvail().x;
		const ImVec2 cursor = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddLine( ImVec2( cursor.x + a_OffsetX, cursor.y + a_OffsetY ),
											 ImVec2( cursor.x + width, cursor.y + a_OffsetY ),
											 GetTheme().Underline, 1.0f);

		if ( a_SpanFullWidth )
		{
			if ( ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr )
				ImGui::PopColumnsBackground();
			else if ( ImGui::GetCurrentTable() != nullptr )
				ImGui::TablePopBackgroundChannel();
		}
	}

	void DrawSmartBezier( const Vector2& a_Start, const Vector2& a_End, UIColor a_Color, float a_Thickness )
	{
		const ImVec2 start = ImGui::Convert( a_Start );
		const ImVec2 end = ImGui::Convert( a_End );

		const float distance = sqrt( pow( ( end.x - start.x ), 2.f ) + pow( ( end.y - start.y ), 2.f ) );
		float delta = distance * 0.45f;

		if ( end.x < start.x )
			delta += 0.2f * ( start.x - end.x );

		// float vert = (end.x < start.x - 20.f) ? 0.062f * distance * (end.y - start.y) * 0.005f : 0.f;

		const float vert = 0.f;
		ImVec2 end2 = end - ImVec2( delta, vert );

		if ( end.x < start.x - 50.f )
			delta *= -1.f;

		const ImVec2 start1 = start + ImVec2( delta, vert );
		ImGui::GetWindowDrawList()->AddBezierCubic( start, start1, end2, end, a_Color, a_Thickness );
	}

	bool SmartBezierCollision( const Vector2& a_Start, const Vector2& a_End, const Vector2& a_Point, float a_Radius )
	{
		const float Distance = sqrt( pow( ( a_End.X - a_Start.X ), 2.f ) + pow( ( a_End.Y - a_Start.Y ), 2.f ) );
		float Delta = Distance * 0.45f;

		if ( a_End.X < a_Start.X )
			Delta += 0.2f * ( a_Start.X - a_End.X );

		// float vert = (a_End.X < a_Start.X - 20.f) ? 0.062f * Distance * (a_End.Y - a_Start.Y) * 0.005f : 0.f;

		float vert = 0.f;
		Vector2 p22 = a_End - Vector2( Delta, vert );

		if ( a_End.X < a_Start.X - 50.f )
			Delta *= -1.f;

		Vector2 p11 = a_Start + Vector2( Delta, vert );

		return ::Util::ImProjectOnCubicBezier(
			ImGui::Convert( a_Point ), ImGui::Convert( a_Start ),
			ImGui::Convert( p11 ), ImGui::Convert( p22 ),
			ImGui::Convert( a_End ) ).Distance < a_Radius;
	}

} // namespace Tridium::UI

#endif // CONFIG_ENABLE_TOOL_UI