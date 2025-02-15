#include "tripch.h"
#include "ImGui.h"
#include <Tridium/Engine/Engine.h>
#include "imgui_internal.h"

float Tridium::s_FontSize = 17.85f;

namespace ImGui {

    struct InputTextCallback_UserData
    {
        std::string* Str;
        ImGuiInputTextCallback  ChainCallback;
        void* ChainCallbackUserData;
    };

    static int InputTextCallback( ImGuiInputTextCallbackData* data )
    {
        InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
        if ( data->EventFlag == ImGuiInputTextFlags_CallbackResize )
        {
            // Resize string callback
            // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
            std::string* str = user_data->Str;
            IM_ASSERT( data->Buf == str->c_str() );
            str->resize( data->BufTextLen );
            data->Buf = (char*)str->c_str();
        }
        else if ( user_data->ChainCallback )
        {
            // Forward to user callback, if any
            data->UserData = user_data->ChainCallbackUserData;
            return user_data->ChainCallback( data );
        }
        return 0;
    }

    bool ImGui::InputText( const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data )
    {
        IM_ASSERT( ( flags & ImGuiInputTextFlags_CallbackResize ) == 0 );
        flags |= ImGuiInputTextFlags_CallbackResize;

        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str = str;
        cb_user_data.ChainCallback = callback;
        cb_user_data.ChainCallbackUserData = user_data;
        return InputText( label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data );
    }

    bool ImGui::InputTextMultiline( const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data )
    {
        IM_ASSERT( ( flags & ImGuiInputTextFlags_CallbackResize ) == 0 );
        flags |= ImGuiInputTextFlags_CallbackResize;

        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str = str;
        cb_user_data.ChainCallback = callback;
        cb_user_data.ChainCallbackUserData = user_data;
        return InputTextMultiline( label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data );
    }

    bool ImGui::InputTextWithHint( const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data )
    {
        IM_ASSERT( ( flags & ImGuiInputTextFlags_CallbackResize ) == 0 );
        flags |= ImGuiInputTextFlags_CallbackResize;

        InputTextCallback_UserData cb_user_data;
        cb_user_data.Str = str;
        cb_user_data.ChainCallback = callback;
        cb_user_data.ChainCallbackUserData = user_data;
        return InputTextWithHint( label, hint, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data );
    }

    bool IconButton( const char* icon, const ImVec2& size )
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if ( window->SkipItems )
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID( icon );
        const ImVec2 label_size = ImGui::CalcTextSize( icon, nullptr, true );
        const ImGuiButtonFlags flags = 0;

        // Determine button position and size
        const ImVec2 pos = window->DC.CursorPos;
		const float textSize = Tridium::Math::Max( label_size.x, label_size.y );
		const ImVec2 itemSize = ImGui::CalcItemSize( size, textSize + style.FramePadding.x * 2.0f, textSize + style.FramePadding.y * 2.0f );
        const ImRect bb( pos, pos + itemSize );

        ImGui::ItemSize( itemSize, 0.0f );
        if ( !ImGui::ItemAdd( bb, id ) )
            return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, flags );

        // Render button background
        const ImU32 col = ImGui::GetColorU32( ( held && hovered ) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button );
        ImGui::RenderNavHighlight( bb, id );
        ImGui::RenderFrame( bb.Min, bb.Max, col, true, style.FrameRounding );

        // Correctly center the icon inside the button
        const ImVec2 label_pos = ImVec2(
			bb.Min.x + ( bb.GetWidth() - label_size.x ) * 0.5f + style.FramePadding.x,
			bb.Min.y
        );
        ImGui::RenderText( label_pos, icon );

        // Debug info for ImGui testing
        IMGUI_TEST_ENGINE_ITEM_INFO( id, icon, g.LastItemData.StatusFlags );

        return pressed;
    }


    bool BorderedSelectable( const char* label, bool selected, ImGuiSelectableFlags flags, const float borderThickness, ImU32 borderColor, float rounding, const ImVec2& size )
	{
		// Get the ImGui window draw list
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Draw the selectable
		bool wasSelected = ImGui::Selectable( label, selected, flags, size );

		// Get the position and size of the last item
		ImVec2 itemMin = ImGui::GetItemRectMin();
		ImVec2 itemMax = ImGui::GetItemRectMax();

		// Adjust the position for the border to be centered correctly
		ImVec2 borderMin = ImVec2( itemMin.x - borderThickness * 0.5f * 0.5f, itemMin.y - borderThickness * 0.5f * 0.5f );
		ImVec2 borderMax = ImVec2( itemMax.x + borderThickness * 0.5f * 0.5f, itemMax.y + borderThickness * 0.5f * 0.5f );

		// Draw the border
		draw_list->AddRect( borderMin, borderMax, borderColor, rounding, 0, borderThickness );

		return wasSelected;
	}

	ImFont* GetLightFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Engine::Get()->GetEngineAssetsDirectory() / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-Light.ttf" ).ToString().c_str(), Tridium::s_FontSize );

		return s_Font;
	}
	ImFont* GetRegularFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Engine::Get()->GetEngineAssetsDirectory() / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-Regular.ttf" ).ToString().c_str(), Tridium::s_FontSize );

		return s_Font;
	}
	ImFont* GetBoldFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Engine::Get()->GetEngineAssetsDirectory() / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-Bold.ttf" ).ToString().c_str(), Tridium::s_FontSize );

		return s_Font;
	}
	ImFont* GetExtraBoldFont()
	{
		static ImFont* s_Font = GetIO().Fonts->AddFontFromFileTTF( ( Tridium::Engine::Get()->GetEngineAssetsDirectory() / "Fonts" / "JetBrainsMono-2.304" / "fonts" / "ttf" / "JetBrainsMonoNL-ExtraBold.ttf" ).ToString().c_str(), Tridium::s_FontSize );

		return s_Font;
	}

    bool IsItemActive( ImGuiID id )
    {
		ImGuiContext& g = *GImGui;
        return g.ActiveId == id;
    }
}