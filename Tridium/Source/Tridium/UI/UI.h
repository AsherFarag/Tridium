#pragma once
#include <Tridium/UI/Config.h>

#if CONFIG_ENABLE_TOOL_UI

#include <Tridium/Core/Enum.h>
#include <Tridium/ImGui/ImGui.h>
#include <Tridium/Math/Vector.h>
#include <Tridium/UI/UITheme.h>

namespace Tridium {
	
	//=============================================================================================
	// Arithmetic Kind: Enumeration of supported arithmetic types.
	//=============================================================================================
	enum class EArithmeticKind : uint8_t
	{
		None,
		Bool,
		Float32,
		Float64,
		Int8,
		Int16,
		Int32,
		Int64,
		UInt8,
		UInt16,
		UInt32,
		UInt64,
	};

	template < typename T >
	consteval EArithmeticKind GetArithmeticKind()
	{
		if constexpr ( std::is_same_v< T, bool		> ) return EArithmeticKind::Bool;
		if constexpr ( std::is_same_v< T, float		> ) return EArithmeticKind::Float32;
		if constexpr ( std::is_same_v< T, double	> ) return EArithmeticKind::Float64;
		if constexpr ( std::is_same_v< T, int8_t	> ) return EArithmeticKind::Int8;
		if constexpr ( std::is_same_v< T, int16_t	> ) return EArithmeticKind::Int16;
		if constexpr ( std::is_same_v< T, int32_t	> ) return EArithmeticKind::Int32;
		if constexpr ( std::is_same_v< T, int64_t	> ) return EArithmeticKind::Int64;
		if constexpr ( std::is_same_v< T, uint8_t	> ) return EArithmeticKind::UInt8;
		if constexpr ( std::is_same_v< T, uint16_t	> ) return EArithmeticKind::UInt16;
		if constexpr ( std::is_same_v< T, uint32_t	> ) return EArithmeticKind::UInt32;
		if constexpr ( std::is_same_v< T, uint64_t	> ) return EArithmeticKind::UInt64;

		else return EArithmeticKind::None;
	}

	constexpr size_t GetSize( EArithmeticKind a_Kind )
	{
		switch ( a_Kind )
		{
			case EArithmeticKind::Bool:		return sizeof( bool );
			case EArithmeticKind::Float32:	return sizeof( float );
			case EArithmeticKind::Float64:	return sizeof( double );
			case EArithmeticKind::Int8:		return sizeof( int8_t );
			case EArithmeticKind::Int16:	return sizeof( int16_t );
			case EArithmeticKind::Int32:	return sizeof( int32_t );
			case EArithmeticKind::Int64:	return sizeof( int64_t );
			case EArithmeticKind::UInt8:	return sizeof( uint8_t );
			case EArithmeticKind::UInt16:	return sizeof( uint16_t );
			case EArithmeticKind::UInt32:	return sizeof( uint32_t );
			case EArithmeticKind::UInt64:	return sizeof( uint64_t );
			default:						return 0;
		}
	}

namespace UI {

	struct UIState
	{
		// Unique ID that is incremented and decremented with each PushID/PopID call.
		int UniqueIDStack = 0;
		int IDCounter = 0;

		// Used to track if a property grid is currently open, for proper ID management.
		int32_t PropertyGridStackCounter = 0;

		bool IsDisabled() const
		{
			ImGuiContext& g = *GImGui;
			return g.CurrentItemFlags & ImGuiItemFlags_Disabled;
		}

		bool IsPropertyGridOpen() const
		{
			return PropertyGridStackCounter > 0;
		}
	};

	UIState& GetUIState();

	//=============================================================================================
	// Moves the drawing cursor by the specified offset.
	void ShiftCursor( float a_OffsetX, float a_OffsetY );

	//=============================================================================================
	// Generates and pushes unique ID.
	// WARNING: PushID must have a matching PopID call.
	void PushID();
	void PopID();
	struct ScopedID
	{
		ScopedID() { PushID(); }
		~ScopedID() { PopID(); }
	};

	//=============================================================================================
	inline const char* GenerateID()
	{
		static thread_local char s_IDBuffer[16 + 2 + 1] = "##";
		snprintf( s_IDBuffer + 2, 16, "%u", GetUIState().IDCounter++ );
		return s_IDBuffer;
	}

	//=============================================================================================
	inline void BeginDisabled( bool a_Disabled = true ) { ImGui::BeginDisabled( a_Disabled ); }
	inline void EndDisabled() { ImGui::EndDisabled(); }
	struct ScopedDisabled
	{
		ScopedDisabled( bool a_Disabled = true ) { ImGui::BeginDisabled( a_Disabled ); }
		~ScopedDisabled() { ImGui::EndDisabled(); }
	};

	struct ScopedOverrideEnable
	{
		ScopedOverrideEnable() { WasDisabled = UI::GetUIState().IsDisabled(); if ( WasDisabled ) ImGui::BeginDisabledOverrideReenable(); }
		~ScopedOverrideEnable() { if ( WasDisabled ) ImGui::EndDisabledOverrideReenable(); }
		bool WasDisabled = false;
	};

	//=============================================================================================
	// Draw Functions
	//=============================================================================================

	//=============================================================================================
	// Draws a horizontal line under the current cursor position.
	// If a_SpanFullWidth is true, the line will span the full available content width.
	void DrawUnderline( bool a_SpanFullWidth = false, float a_OffsetX = 0.0f, float a_OffsetY = 0.0f );

	//=============================================================================================
	inline void SeparatorText( StringView a_Label )
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if ( window->SkipItems )
			return;

		ImGui::SeparatorTextEx( 0, a_Label.data(), a_Label.data() + a_Label.size(), 0.0f);
	}

	//=============================================================================================
	// Fast draw function for a simple bezier curve between two points.
	// Use SmartBezierCollision to check for collisions with the curve.
	void DrawSmartBezier( const Vector2& a_Start, const Vector2& a_End, UIColor a_Color, float a_Thickness = 1.0f );
	bool SmartBezierCollision( const Vector2& a_Start, const Vector2& a_End, const Vector2& a_Point, float a_Radius );

	//=============================================================================================
	// Property Grid Functions
	// Any DrawProperty call between BeginPropertyGrid and EndPropertyGrid,
	// will be drawn in a property grid format.
	// a_NumColumns specifies how many columns the grid will have.
	// WARNING: BeginPropertyGrid must have a matching EndPropertyGrid call.
	void BeginPropertyGrid( uint32_t a_NumColumns = 2 );
	void EndPropertyGrid();
	inline bool IsPropertyGridOpen() { return GetUIState().IsPropertyGridOpen(); }

	//=============================================================================================
	inline Vector2 CalcButtonSize( StringView a_Label )
	{
		ImVec2 textSize = ImGui::CalcTextSize( a_Label.data(), a_Label.data() + a_Label.size() );
		ImVec2 padding = ImGui::GetStyle().FramePadding;
		return Vector2( textSize.x + padding.x * 2.0f, textSize.y + padding.y * 2.0f );
	}

	//=============================================================================================
	enum class ETreeFlags
	{
		None = ImGuiTreeNodeFlags_None,
		Selected = ImGuiTreeNodeFlags_Selected,                       
		Framed = ImGuiTreeNodeFlags_Framed,                           
		AllowOverlap = ImGuiTreeNodeFlags_AllowOverlap,               
		NoTreePushOnOpen = ImGuiTreeNodeFlags_NoTreePushOnOpen,       
		NoAutoOpenOnLog = ImGuiTreeNodeFlags_NoAutoOpenOnLog,         
		DefaultOpen = ImGuiTreeNodeFlags_DefaultOpen,                 
		OpenOnDoubleClick = ImGuiTreeNodeFlags_OpenOnDoubleClick,     
		OpenOnArrow = ImGuiTreeNodeFlags_OpenOnArrow,                 
		Leaf = ImGuiTreeNodeFlags_Leaf,                               
		Bullet = ImGuiTreeNodeFlags_Bullet,                           
		FramePadding = ImGuiTreeNodeFlags_FramePadding,               
		SpanAvailWidth = ImGuiTreeNodeFlags_SpanAvailWidth,           
		SpanFullWidth = ImGuiTreeNodeFlags_SpanFullWidth,             
		SpanLabelWidth = ImGuiTreeNodeFlags_SpanLabelWidth,           
		SpanAllColumns = ImGuiTreeNodeFlags_SpanAllColumns,           
		LabelSpanAllColumns = ImGuiTreeNodeFlags_LabelSpanAllColumns, 
		DrawLinesNone = ImGuiTreeNodeFlags_DrawLinesNone,
		DrawLinesFull = ImGuiTreeNodeFlags_DrawLinesFull,
		DrawLinesToNodes = ImGuiTreeNodeFlags_DrawLinesToNodes,
	};
	DEFINE_ENUM_BITMASK_OPERATORS( ETreeFlags );

	//=============================================================================================
	bool BeginTree( StringView a_Label, ETreeFlags a_Flags = ETreeFlags::None, float a_Width = 0.0f );
	void EndTree();

	//=============================================================================================
	template<typename _Func>
	bool DrawGridProperty( const StringView& a_Name, _Func&& a_DrawFunc )
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex( 0 );
		ImGui::AlignTextToFramePadding();

		{
			ScopedOverrideEnable enableLabel{};
			ImGui::TextUnformatted( a_Name.data(), a_Name.data() + a_Name.size() );
		}

		ImGui::TableSetColumnIndex( 1 );
		ImGui::PushItemWidth( -FLT_MIN );

		bool modified = a_DrawFunc();

		ImGui::PopItemWidth();
		//DrawUnderline();

		return modified;
	}

	//=============================================================================================
	bool DrawCheckbox( StringView a_Label, bool& a_Value );

	//=============================================================================================
	bool DrawInputText( StringView a_Label, String& a_Text );
	bool DrawInputTextMultiline( StringView a_Label, String& a_Text );


	//=============================================================================================
	bool DrawScalar( StringView a_Label, EArithmeticKind a_Kind, void* a_Data,
					 const void* a_Min = nullptr, const void* a_Max = nullptr );

	template<typename T>
	bool DrawScalar( StringView a_Label, T& a_Value,
					 const T* a_Min = nullptr, const T* a_Max = nullptr )
	{
		return DrawScalar( a_Label, GetArithmeticKind<T>(), Cast<void*>( &a_Value ),
						   a_Min ? Cast<const void*>( a_Min ) : nullptr,
						   a_Max ? Cast<const void*>( a_Max ) : nullptr );
	}

	//=============================================================================================
	bool DrawVector( StringView a_Label, EArithmeticKind a_Kind, void* a_Data, uint32_t a_ComponentCount,
					 const void* a_Min = nullptr, const void* a_Max = nullptr );

	template<typename T, size_t N>
	bool DrawVector( StringView a_Label, Vector<N, T>& a_Vector,
					 const T* a_Min = nullptr, const T* a_Max = nullptr )
	{
		return DrawVector( a_Label, GetArithmeticKind<T>(), Cast<void*>( &a_Vector ), (uint32_t)N,
						   a_Min ? Cast<const void*>( a_Min ) : nullptr,
						   a_Max ? Cast<const void*>( a_Max ) : nullptr );
	}

} // namespace UI

} // namespace Tridium

#endif // CONFIG_ENABLE_TOOL_UI