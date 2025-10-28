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

	static ImGuiDataType ToImGui( EArithmeticKind a_Type )
	{
		switch ( a_Type )
		{
			default:
			case EArithmeticKind::None: ASSERT( false, "Invalid scalar type" ); return ImGuiDataType_::ImGuiDataType_Float;
			case EArithmeticKind::Bool:		 return ImGuiDataType_Bool;
			case EArithmeticKind::Float32:   return ImGuiDataType_Float;
			case EArithmeticKind::Float64:   return ImGuiDataType_Double;
			case EArithmeticKind::Int8: 	 return ImGuiDataType_S8;
			case EArithmeticKind::Int16: 	 return ImGuiDataType_S16;
			case EArithmeticKind::Int32: 	 return ImGuiDataType_S32;
			case EArithmeticKind::Int64: 	 return ImGuiDataType_S64;
			case EArithmeticKind::UInt8: 	 return ImGuiDataType_U8;
			case EArithmeticKind::UInt16:	 return ImGuiDataType_U16;
			case EArithmeticKind::UInt32: 	 return ImGuiDataType_U32;
			case EArithmeticKind::UInt64: 	 return ImGuiDataType_U64;
		}
	}

	UIState& GetUIState()
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

	void BeginPropertyGrid( uint32_t a_NumColumns )
	{
		PushID();
		ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 8, 4 ) );

		if ( ImGui::BeginTable( "", a_NumColumns,
								ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable ) )
		{
			GetUIState().PropertyGridStackCounter++;
		}
	}

	void EndPropertyGrid()
	{
		ImGui::EndTable();
		ImGui::PopStyleVar();
		//DrawUnderline();
		//ImGui::Dummy( {} );
		PopID();

		GetUIState().PropertyGridStackCounter--;
		ASSERT( GetUIState().PropertyGridStackCounter >= 0,
			"EndPropertyGrid called more times than BeginPropertyGrid" );
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

	bool BeginTree( StringView a_Label, ETreeFlags a_Flags, float a_Width )
	{
		if ( a_Width <= 0.0f )
		{
			return ImGui::TreeNodeEx( a_Label.data(), Cast<ImGuiTreeNodeFlags>( a_Flags ), "%.*s", (int)a_Label.size(), a_Label.data() );
		}

		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGui::SetNextItemWidth( a_Width );
		float backupWorkMaxX = window->WorkRect.Max.x;
		window->WorkRect.Max.x = window->DC.CursorPos.x + ImGui::CalcItemWidth();
		bool ret = ImGui::TreeNodeEx( a_Label.data(), Cast<ImGuiTreeNodeFlags>( a_Flags ), "%.*s", (int)a_Label.size(), a_Label.data() );
		window->WorkRect.Max.x = backupWorkMaxX;
		return ret;
	}

	void EndTree()
	{
		ImGui::TreePop();
	}

	bool DrawCheckbox( StringView a_Label, bool& a_Value )
	{
		if ( !GetUIState().IsPropertyGridOpen() )
		{
			return ImGui::Checkbox( a_Label.data(), &a_Value );
		}
		else
		{
			return DrawGridProperty( a_Label, [&]() { return ImGui::Checkbox( GenerateID(), &a_Value ); } );
		}
	}

	bool DrawInputText( StringView a_Label, String& a_Text )
	{
		if ( !GetUIState().IsPropertyGridOpen() )
		{
			return ImGui::InputText( a_Label.data(), &a_Text );
		}
		else
		{
			return DrawGridProperty( a_Label, [&]() { return ImGui::InputText( GenerateID(), &a_Text ); } );
		}
	}

	bool DrawInputTextMultiline( StringView a_Label, String& a_Text )
	{
		if ( !GetUIState().IsPropertyGridOpen() )
		{
			return ImGui::InputTextMultiline( a_Label.data(), &a_Text );
		}
		else
		{
			return DrawGridProperty( a_Label, [&]() { return ImGui::InputTextMultiline( GenerateID(), &a_Text ); } );
		}
	}

	bool DrawScalar( StringView a_Label, EArithmeticKind a_Kind, void* a_Data, const void* a_Min, const void* a_Max )
	{
		const auto DrawScalar = [&]()
		{
			const ImGuiDataType imGuiType = ToImGui( a_Kind );
			const bool isSlider = ( a_Min && a_Max );

			if ( ImGui::GetCurrentWindow()->SkipItems )
				return false;

			const char* ID = GenerateID();
			bool valueChanged = isSlider
				? ImGui::SliderScalar( ID, imGuiType, a_Data, a_Min, a_Max )
				: ImGui::DragScalar( ID, imGuiType, a_Data );

			// If we are not in a property grid, we can draw the label after the scalar.
			if ( !GetUIState().IsPropertyGridOpen() )
			{
				const char* LabelEnd = ImGui::FindRenderedTextEnd( a_Label.data(), a_Label.data() + a_Label.size() );
				if ( a_Label.data() != LabelEnd )
				{
					ImGui::SameLine( 0, GImGui->Style.ItemInnerSpacing.x );
					ImGui::TextEx( a_Label.data(), LabelEnd );
				}
			}

			return valueChanged;
		};

		if ( !GetUIState().IsPropertyGridOpen() )
		{
			return DrawScalar();
		}
		else
		{
			return DrawGridProperty( a_Label, DrawScalar );
		}
	}

	bool DrawVector( StringView a_Label, EArithmeticKind a_Type, void* a_Data, uint32_t a_Count, const void* a_Min, const void* a_Max )
	{
		static constexpr uint32_t MaxComponents = 4; // Max components for a vector (X, Y, Z, W)
		ASSERT( a_Count > 0 && a_Count <= MaxComponents, "Invalid vector count: {}", a_Count );

		const auto DrawVector = [&]()
		{
			const ImGuiDataType imGuiType = ToImGui( a_Type );
			const bool isSlider = ( a_Min && a_Max );
			const uint32_t imGuiTypeSize = (uint32_t)GetSize( a_Type );

			if ( ImGui::GetCurrentWindow()->SkipItems )
				return false;

			bool valueChanged = false;

			ImGui::BeginGroup();
			{
				ImGui::ScopedStyleVar ScopedStyleBorderSize{ ImGuiStyleVar_FrameBorderSize, 1.0f };
				const ImU32 VectorColors[MaxComponents] = {
					GetTheme().Red,   // X
					GetTheme().Green, // Y
					GetTheme().Blue,  // Z
					GetTheme().Orange // W
				};

				ImGui::PushMultiItemsWidths( a_Count, ImGui::CalcItemWidth() );

				// Render each component of the vector. X, Y, Z, W
				for ( uint32_t i = 0; i < a_Count; i++ )
				{
					ImGui::ScopedStyleCol ScopedStyleCol{ ImGuiCol_Border, VectorColors[i] };

					void* ComponentValue = Cast<uint8_t*>( a_Data ) + ( i * imGuiTypeSize );
					const char* ID = GenerateID();

					if ( i > 0 ) ImGui::SameLine( 0, GImGui->Style.ItemInnerSpacing.x );

					valueChanged |= isSlider
						? ImGui::SliderScalar( ID, imGuiType, ComponentValue, a_Min, a_Max )
						: ImGui::DragScalar( ID, imGuiType, ComponentValue );

					ImGui::PopItemWidth();
				}

				// If we are not in a property grid, we can draw the label after the vector components.
				if ( !GetUIState().IsPropertyGridOpen() )
				{
					const char* LabelEnd = ImGui::FindRenderedTextEnd( a_Label.data(), a_Label.data() + a_Label.size() );

					if ( a_Label.data() != LabelEnd )
					{
						ImGui::SameLine( 0, GImGui->Style.ItemInnerSpacing.x );
						ImGui::TextEx( a_Label.data(), LabelEnd );
					}
				}
			}
			ImGui::EndGroup();

			return valueChanged;
		};

		if ( !GetUIState().IsPropertyGridOpen() )
		{
			return DrawVector();
		}
		else
		{
			return DrawGridProperty( a_Label, DrawVector );
		}
	}

} // namespace Tridium::UI

#endif // CONFIG_ENABLE_TOOL_UI