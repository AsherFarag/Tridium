#pragma once
#include <Tridium/Math/Vector.h>
#include <Tridium/Editor/EditorIcons.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"	
#include "imgui_stdlib.h"
#undef IMGUI_DEFINE_MATH_OPERATORS

#define TE_PAYLOAD_CONTENT_BROWSER_ITEM "ContentBrowserItem"
#define TE_PAYLOAD_ASSET_HANDLE "AssetHandle"
#define TE_PAYLOAD_GAME_OBJECT "GameObject"

namespace Tridium {

	extern float s_FontSize;

}

inline ImVec2 operator*( const float lhs, const ImVec2& rhs )
{
	return ImVec2( lhs * rhs.x, lhs * rhs.y );
}

namespace ImGui {

	inline ImVec2 Convert( const Tridium::Vector2& a_Vec ) { return ImVec2( a_Vec.X, a_Vec.Y ); }
	inline ImVec4 Convert( const Tridium::Vector4& a_Vec ) { return ImVec4( a_Vec.X, a_Vec.Y, a_Vec.Z, a_Vec.W ); }
	inline Tridium::Vector2 Convert( const ImVec2& a_Vec ) { return Tridium::Vector2( a_Vec.x, a_Vec.y ); }
	inline Tridium::Vector4 Convert( const ImVec4& a_Vec ) { return Tridium::Vector4( a_Vec.x, a_Vec.y, a_Vec.z, a_Vec.w ); }

	template<typename _Func>
	struct FunctionScope
	{
		FunctionScope( const _Func& a_Function )
			: Function( a_Function )
		{
		}

		~FunctionScope()
		{
			Function();
		}

		_Func Function;
	};

	ImFont* GetLightFont();
	ImFont* GetRegularFont();
	ImFont* GetBoldFont();
	ImFont* GetExtraBoldFont();

	bool IsItemActive( ImGuiID id );

	//=================================================================================================
	// Buttons
	//=================================================================================================

	// Draws a button with an icon so that the icon is centered
	bool IconButton( const char* icon, const ImVec2& size = { 0, 0 } );

	//=================================================================================================

	bool BorderedSelectable( const char* label, bool selected, ImGuiSelectableFlags flags = 0, const float borderThickness = 1.0f, ImU32 borderColor = IM_COL32( 255, 255, 255, 255 ), float rounding = 0.0f, const ImVec2& size = ImVec2( 0, 0 ) );
	template <typename PayloadFunction >
	bool DragDropSelectable( const char* label, bool isValid, const char* text, const char* payloadType, PayloadFunction payloadFunction )
	{
		ImGui::PushFont( ImGui::GetBoldFont() );
		ImGui::Text( label );
		ImGui::PopFont();

		bool wasDoubleClicked = false;

		ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_AllowDoubleClick;
		selectableFlags |= !isValid ? ImGuiSelectableFlags_Disabled : 0;

		ImGui::SameLine();
		ImGui::PushFont( ImGui::GetLightFont() );
		isValid ? ImGui::PushStyleColor( ImGuiCol_Text, { 0.85, 0.65, 0.1, 0.9 } ) : ImGui::PushStyleColor( ImGuiCol_Text, { 0.65, 0.65, 0.65, 0.9 } );
		if ( ImGui::BorderedSelectable( text, false, selectableFlags, 1.f, IM_COL32( 255, 255, 255, 255 ), 2.f )
			&& ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
		{
			wasDoubleClicked = true;
		}
		ImGui::PopStyleColor();
		ImGui::PopFont();

		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( payloadType ) )
			{
				payloadFunction( payload );
			}
			ImGui::EndDragDropTarget();
		}

		return wasDoubleClicked;
	}

	struct ScopedDragDropTarget
	{
	public:
		ScopedDragDropTarget() { m_Successful = ImGui::BeginDragDropTarget(); }
		~ScopedDragDropTarget() { if ( m_Successful ) ImGui::EndDragDropTarget(); }

		operator bool() { return m_Successful; }
		operator const bool() const { return m_Successful; }

	private:
		bool m_Successful = false;
	};

	struct ScopedStyleVar
	{
		ScopedStyleVar( ImGuiStyleVar_ styleVar, float val ) { ImGui::PushStyleVar( styleVar, val ); }
		ScopedStyleVar( ImGuiStyleVar_ styleVar, ImVec2 val ) { ImGui::PushStyleVar( styleVar, val ); }
		~ScopedStyleVar() { ImGui::PopStyleVar(); }
	};

	struct ScopedStyleCol
	{
		ScopedStyleCol( ImGuiCol styleCol, ImVec4 col ) { ImGui::PushStyleColor( styleCol, col ); }
		ScopedStyleCol( ImGuiCol styleCol, ImU32 col ) { ImGui::PushStyleColor( styleCol, col ); }
		~ScopedStyleCol() { ImGui::PopStyleColor(); }
	};

	struct ScopedID
	{
		ScopedID( int a_ID ) { ImGui::PushID( a_ID ); }
		ScopedID( const char* a_ID ) { ImGui::PushID( a_ID ); }
		ScopedID( const char* str_id_begin, const char* str_id_end ) { ImGui::PushID( str_id_begin, str_id_end ); }
		ScopedID( const void* a_ID ) { ImGui::PushID( a_ID ); }
		~ScopedID() { ImGui::PopID(); }
	};

	static bool BeginMenuBarEx( const ImRect& a_BarRectangle )
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if ( window->SkipItems )
			return false;

		IM_ASSERT( !window->DC.MenuBarAppending );
		ImGui::BeginGroup(); // Backup position on layer 0 // FIXME: Misleading to use a group for that backup/restore
		ImGui::PushID( "##menubar" );

		const ImVec2 padding = window->WindowPadding;

		// We don't clip with current window clipping rectangle as it is already set to the area below. However we clip with window full rect.
		// We remove 1 worth of rounding to Max.x to that text in long menus and small windows don't tend to display over the lower-right rounded area, which looks particularly glitchy.
		ImRect bar_rect = ImRect(
			a_BarRectangle.Min.x, a_BarRectangle.Min.y + padding.y,
			a_BarRectangle.Max.x, a_BarRectangle.Max.y + padding.y );
		ImRect clip_rect( IM_ROUND( ImMax( window->Pos.x, bar_rect.Min.x + window->WindowBorderSize + /*window->Pos.x*/ -10.0f ) ), IM_ROUND( bar_rect.Min.y + window->WindowBorderSize /*+ window->Pos.y*/ ),
						  IM_ROUND( ImMax( bar_rect.Min.x /*+ window->Pos.x*/, bar_rect.Max.x - ImMax( window->WindowRounding, window->WindowBorderSize ) ) ), IM_ROUND( bar_rect.Max.y /*+ window->Pos.y*/ ) );

		clip_rect.ClipWith( window->OuterRectClipped );
		ImGui::PushClipRect( clip_rect.Min, clip_rect.Max, false );

		// We overwrite CursorMaxPos because BeginGroup sets it to CursorPos (essentially the .EmitItem hack in EndMenuBar() would need something analogous here, maybe a BeginGroupEx() with flags).
		window->DC.CursorPos = window->DC.CursorMaxPos = ImVec2( bar_rect.Min.x /*+ window->Pos.x*/, bar_rect.Min.y /*+ window->Pos.y*/ );
		window->DC.LayoutType = ImGuiLayoutType_Horizontal;
		window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
		window->DC.MenuBarAppending = true;
		ImGui::AlignTextToFramePadding();
		return true;
	}

	static void EndMenuBarEx()
	{
		ImGuiWindow* Window = ImGui::GetCurrentWindow();

		if ( Window->SkipItems )
		{
			return;
		}

		ImGuiContext& Context = *GImGui;

		// Nav: When a move request within one of our child menu failed, capture the request to navigate among our siblings.
		if ( ImGui::NavMoveRequestButNoResultYet() && ( Context.NavMoveDir == ImGuiDir_Left || Context.NavMoveDir == ImGuiDir_Right ) && ( Context.NavWindow->Flags & ImGuiWindowFlags_ChildMenu ) )
		{
			// Try to find out if the request is for one of our child menu
			ImGuiWindow* NavEarliestChild = Context.NavWindow;
			while ( NavEarliestChild->ParentWindow && ( NavEarliestChild->ParentWindow->Flags & ImGuiWindowFlags_ChildMenu ) )
			{
				NavEarliestChild = NavEarliestChild->ParentWindow;
			}

			if ( NavEarliestChild->ParentWindow == Window && NavEarliestChild->DC.ParentLayoutType == ImGuiLayoutType_Horizontal && ( Context.NavMoveFlags & ImGuiNavMoveFlags_Forwarded ) == 0 )
			{
				// To do so we claim focus back, restore NavId and then process the movement request for yet another frame.
				// This involve a one-frame delay which isn't very problematic in this situation. We could remove it by scoring in advance for multiple window (probably not worth bothering)
				const ImGuiNavLayer Layer = ImGuiNavLayer_Menu;
				IM_ASSERT( Window->DC.NavLayersActiveMaskNext & ( 1 << Layer ) ); // Sanity check
				ImGui::FocusWindow( Window );
				ImGui::SetNavID( Window->NavLastIds[Layer], Layer, 0, Window->NavRectRel[Layer] );
				//g.NavDisableHighlight = true; // Hide highlight for the current frame so we don't see the intermediary selection.
				//g.NavDisableMouseHover = g.NavMousePosDirty = true;
				ImGui::NavMoveRequestForward( Context.NavMoveDir, Context.NavMoveClipDir, Context.NavMoveFlags, Context.NavMoveScrollFlags ); // Repeat
			}
		}

		IM_MSVC_WARNING_SUPPRESS( 6011 ); // Static Analysis false positive "warning C6011: Dereferencing NULL pointer 'window'"
		//IM_ASSERT( Window->Flags & ImGuiWindowFlags_MenuBar );
		IM_ASSERT( Window->DC.MenuBarAppending );

		ImGui::PopClipRect();
		ImGui::PopID();

		Window->DC.MenuBarOffset.x = Window->DC.CursorPos.x - Window->Pos.x; // Save horizontal position so next append can reuse it. This is kinda equivalent to a per-layer CursorPos.
		Context.GroupStack.back().EmitItem = false;

		ImGui::EndGroup(); // Restore position on layer 0

		Window->DC.LayoutType = ImGuiLayoutType_Vertical;
		Window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
		Window->DC.MenuBarAppending = false;
	}

}