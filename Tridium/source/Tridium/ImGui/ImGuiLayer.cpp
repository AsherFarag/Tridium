#include "tripch.h"
#include "ImGuiLayer.h"
#include "Tridium/Input/Input.h"

#include <Tridium/Core/Application.h>
#include <Tridium/Engine/Engine.h>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.cpp"
#include "backends/imgui_impl_glfw.cpp"

namespace Tridium {

	void AddIconsToFont()
	{
		const float iconFontSize = s_FontSize * 2.0f / 3.0f;

		// Merge in icons from Font Awesome
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		icons_config.GlyphMinAdvanceX = iconFontSize;

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF(
			( Engine::Get()->GetEngineAssetsDirectory() / "Fonts" / FONT_ICON_FILE_NAME_FAS ).ToString().c_str(),
			iconFontSize, &icons_config, icons_ranges );
	}

	ImGuiLayer::ImGuiLayer()
		: Layer( "ImGuiLayer" )
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		// - Set up flags -
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// - Set Style -
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			style.WindowRounding = 4.f;
			style.FrameRounding = 2.f;
			style.Colors[ ImGuiCol_WindowBg ].w = 1.f;
		}

		// - Set Default Font -
		ImGui::GetLightFont();
		AddIconsToFont();
		io.FontDefault = ImGui::GetRegularFont();
		AddIconsToFont();
		ImGui::GetBoldFont();
		AddIconsToFont();
		ImGui::GetExtraBoldFont();
		AddIconsToFont();

		GLFWwindow* window = static_cast<GLFWwindow*>( Application::Get()->GetWindow().GetNativeWindow() );

		// Set up Platform/Render bindings
		TODO( "Make this API specific!" );
		ImGui_ImplGlfw_InitForOpenGL( window, true );
		ImGui_ImplOpenGL3_Init( "#version 410" );
	}

	void ImGuiLayer::OnDetach()
	{
		TODO( "Make this API specific!" );
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		TODO( "Make this API specific!" );
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		// Size Initialization
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2( Application::Get()->GetWindow().GetWidth(), Application::Get()->GetWindow().GetHeight() );

		// Rendering
		ImGui::Render();
		TODO( "Make this API specific!" );
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			TODO( "Make this API specific!" );
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent( backup_current_context );
		}
	}

	void ImGuiLayer::OnImGuiDraw()
	{
	}

	ImGuiKey ImGuiLayer::TridiumToImGuiKey( Input::KeyCode key )
	{
		switch ( key )
		{
		case Input::INVALID_KEY: return ImGuiKey_None;
		case Input::KEY_SPACE: return ImGuiKey_Space;
		case Input::KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
		case Input::KEY_COMMA: return ImGuiKey_Comma;
		case Input::KEY_MINUS: return ImGuiKey_Minus;
		case Input::KEY_PERIOD: return ImGuiKey_Period;
		case Input::KEY_SLASH: return ImGuiKey_Slash;
		case Input::KEY_0: return ImGuiKey_0;
		case Input::KEY_1: return ImGuiKey_1;
		case Input::KEY_2: return ImGuiKey_2;
		case Input::KEY_3: return ImGuiKey_3;
		case Input::KEY_4: return ImGuiKey_4;
		case Input::KEY_5: return ImGuiKey_5;
		case Input::KEY_6: return ImGuiKey_6;
		case Input::KEY_7: return ImGuiKey_7;
		case Input::KEY_8: return ImGuiKey_8;
		case Input::KEY_9: return ImGuiKey_9;
		case Input::KEY_SEMICOLON: return ImGuiKey_Semicolon;
		case Input::KEY_EQUAL: return ImGuiKey_Equal;
		case Input::KEY_A: return ImGuiKey_A;
		case Input::KEY_B: return ImGuiKey_B;
		case Input::KEY_C: return ImGuiKey_C;
		case Input::KEY_D: return ImGuiKey_D;
		case Input::KEY_E: return ImGuiKey_E;
		case Input::KEY_F: return ImGuiKey_F;
		case Input::KEY_G: return ImGuiKey_G;
		case Input::KEY_H: return ImGuiKey_H;
		case Input::KEY_I: return ImGuiKey_I;
		case Input::KEY_J: return ImGuiKey_J;
		case Input::KEY_K: return ImGuiKey_K;
		case Input::KEY_L: return ImGuiKey_L;
		case Input::KEY_M: return ImGuiKey_M;
		case Input::KEY_N: return ImGuiKey_N;
		case Input::KEY_O: return ImGuiKey_O;
		case Input::KEY_P: return ImGuiKey_P;
		case Input::KEY_Q: return ImGuiKey_Q;
		case Input::KEY_R: return ImGuiKey_R;
		case Input::KEY_S: return ImGuiKey_S;
		case Input::KEY_T: return ImGuiKey_T;
		case Input::KEY_U: return ImGuiKey_U;
		case Input::KEY_V: return ImGuiKey_V;
		case Input::KEY_W: return ImGuiKey_W;
		case Input::KEY_X: return ImGuiKey_X;
		case Input::KEY_Y: return ImGuiKey_Y;
		case Input::KEY_Z: return ImGuiKey_Z;
		case Input::KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
		case Input::KEY_BACKSLASH: return ImGuiKey_Backslash;
		case Input::KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
		case Input::KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;

		TODO( "Currently returns None for World Keys" );
		case Input::KEY_WORLD_1: return ImGuiKey_None;
		case Input::KEY_WORLD_2: return ImGuiKey_None;

		case Input::KEY_ESCAPE: return ImGuiKey_Escape;
		case Input::KEY_ENTER: return ImGuiKey_Enter;
		case Input::KEY_TAB: return ImGuiKey_Tab;
		case Input::KEY_BACKSPACE: return ImGuiKey_Backspace;
		case Input::KEY_INSERT: return ImGuiKey_Insert;
		case Input::KEY_DELETE: return ImGuiKey_Delete;
		case Input::KEY_RIGHT: return ImGuiKey_RightArrow;
		case Input::KEY_LEFT: return ImGuiKey_LeftArrow;
		case Input::KEY_DOWN: return ImGuiKey_DownArrow;
		case Input::KEY_UP: return ImGuiKey_UpArrow;
		case Input::KEY_PAGE_UP: return ImGuiKey_PageUp;
		case Input::KEY_PAGE_DOWN: return ImGuiKey_PageDown;
		case Input::KEY_HOME: return ImGuiKey_Home;
		case Input::KEY_END: return ImGuiKey_End;
		case Input::KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
		case Input::KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
		case Input::KEY_NUM_LOCK: return ImGuiKey_NumLock;
		case Input::KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
		case Input::KEY_PAUSE: return ImGuiKey_Pause;
		case Input::KEY_F1: return ImGuiKey_F1;
		case Input::KEY_F2: return ImGuiKey_F2;
		case Input::KEY_F3: return ImGuiKey_F3;
		case Input::KEY_F4: return ImGuiKey_F4;
		case Input::KEY_F5: return ImGuiKey_F5;
		case Input::KEY_F6: return ImGuiKey_F6;
		case Input::KEY_F7: return ImGuiKey_F7;
		case Input::KEY_F8: return ImGuiKey_F8;
		case Input::KEY_F9: return ImGuiKey_F9;
		case Input::KEY_F10: return ImGuiKey_F10;
		case Input::KEY_F11: return ImGuiKey_F11;
		case Input::KEY_F12: return ImGuiKey_F12;

		/* Imgui does not have key codes for F13 to F25*/
		case Input::KEY_F13: return ImGuiKey_None;
		case Input::KEY_F14: return ImGuiKey_None;
		case Input::KEY_F15: return ImGuiKey_None;
		case Input::KEY_F16: return ImGuiKey_None;
		case Input::KEY_F17: return ImGuiKey_None;
		case Input::KEY_F18: return ImGuiKey_None;
		case Input::KEY_F19: return ImGuiKey_None;
		case Input::KEY_F20: return ImGuiKey_None;
		case Input::KEY_F21: return ImGuiKey_None;
		case Input::KEY_F22: return ImGuiKey_None;
		case Input::KEY_F23: return ImGuiKey_None;
		case Input::KEY_F24: return ImGuiKey_None;
		case Input::KEY_F25: return ImGuiKey_None;

		case Input::KEY_KP_0: return ImGuiKey_Keypad0;
		case Input::KEY_KP_1: return ImGuiKey_Keypad1;
		case Input::KEY_KP_2: return ImGuiKey_Keypad2;
		case Input::KEY_KP_3: return ImGuiKey_Keypad3;
		case Input::KEY_KP_4: return ImGuiKey_Keypad4;
		case Input::KEY_KP_5: return ImGuiKey_Keypad5;
		case Input::KEY_KP_6: return ImGuiKey_Keypad6;
		case Input::KEY_KP_7: return ImGuiKey_Keypad7;
		case Input::KEY_KP_8: return ImGuiKey_Keypad8;
		case Input::KEY_KP_9: return ImGuiKey_Keypad9;
		case Input::KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
		case Input::KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
		case Input::KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
		case Input::KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
		case Input::KEY_KP_ADD: return ImGuiKey_KeypadAdd;
		case Input::KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
		case Input::KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
		case Input::KEY_LEFT_SHIFT:return ImGuiKey_LeftShift;
		case Input::KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
		case Input::KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
		case Input::KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
		case Input::KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
		case Input::KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
		case Input::KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
		case Input::KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
		case Input::KEY_MENU: return ImGuiKey_Menu;

		default: return ImGuiKey_None;
		}
	}
}
