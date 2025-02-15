#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuiHelpers.h"	

#define TE_PAYLOAD_CONTENT_BROWSER_ITEM "ContentBrowserItem"
#define TE_PAYLOAD_ASSET_HANDLE "AssetHandle"
#define TE_PAYLOAD_GAME_OBJECT "GameObject"

#include "IconsFontAwesome6.h"

namespace Tridium {

	extern float s_FontSize;

}

static ImVec4 operator*( const ImVec4& a_Color, float a_Value )
{
	return ImVec4( a_Color.x * a_Value, a_Color.y * a_Value, a_Color.z * a_Value, a_Color.w );
}

namespace ImGui {

	ImFont* GetLightFont();
	ImFont* GetRegularFont();
	ImFont* GetBoldFont();
	ImFont* GetExtraBoldFont();

	bool IsItemActive( ImGuiID id );

	// ImGui::InputText() with std::string
	// Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
	bool InputText( const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr );
	bool InputTextMultiline( const char* label, std::string* str, const ImVec2& size = ImVec2( 0, 0 ), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr );
	bool InputTextWithHint( const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr );

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

}