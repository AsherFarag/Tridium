#include "tripch.h"
#include "ImGuiLayer.h"
#include "Tridium/Input/Input.h"

#include <Tridium/Core/Application.h>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.cpp"
#include "backends/imgui_impl_glfw.cpp"

namespace Tridium {

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
		ImGuiIO& io = ImGui::GetIO(); (void)io;
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

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>( app.GetWindow().GetNativeWindow() );

		// Set up Platform/Render bindings
		ImGui_ImplGlfw_InitForOpenGL( window, true );
		ImGui_ImplOpenGL3_Init( "#version 410" );
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		// Size Initialization
		ImGuiIO& io = ImGui::GetIO();
		auto& app = Application::Get();
		io.DisplaySize = ImVec2( app.GetWindow().GetWidth(), app.GetWindow().GetHeight() );

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent( backup_current_context );
		}
	}

	void ImGuiLayer::OnImGuiDraw()
	{
		static bool show = true;
		ImGui::ShowDemoWindow( &show );
	}

	ImGuiKey ImGuiLayer::TridiumToImGuiKey( Input::KeyCode key )
	{
		switch ( key )
		{
		case Tridium::Input::INVALID_KEY: return ImGuiKey_None;
		case Tridium::Input::KEY_SPACE: return ImGuiKey_Space;
		case Tridium::Input::KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
		case Tridium::Input::KEY_COMMA: return ImGuiKey_Comma;
		case Tridium::Input::KEY_MINUS: return ImGuiKey_Minus;
		case Tridium::Input::KEY_PERIOD: return ImGuiKey_Period;
		case Tridium::Input::KEY_SLASH: return ImGuiKey_Slash;
		case Tridium::Input::KEY_0: return ImGuiKey_0;
		case Tridium::Input::KEY_1: return ImGuiKey_1;
		case Tridium::Input::KEY_2: return ImGuiKey_2;
		case Tridium::Input::KEY_3: return ImGuiKey_3;
		case Tridium::Input::KEY_4: return ImGuiKey_4;
		case Tridium::Input::KEY_5: return ImGuiKey_5;
		case Tridium::Input::KEY_6: return ImGuiKey_6;
		case Tridium::Input::KEY_7: return ImGuiKey_7;
		case Tridium::Input::KEY_8: return ImGuiKey_8;
		case Tridium::Input::KEY_9: return ImGuiKey_9;
		case Tridium::Input::KEY_SEMICOLON: return ImGuiKey_Semicolon;
		case Tridium::Input::KEY_EQUAL: return ImGuiKey_Equal;
		case Tridium::Input::KEY_A: return ImGuiKey_A;
		case Tridium::Input::KEY_B: return ImGuiKey_B;
		case Tridium::Input::KEY_C: return ImGuiKey_C;
		case Tridium::Input::KEY_D: return ImGuiKey_D;
		case Tridium::Input::KEY_E: return ImGuiKey_E;
		case Tridium::Input::KEY_F: return ImGuiKey_F;
		case Tridium::Input::KEY_G: return ImGuiKey_G;
		case Tridium::Input::KEY_H: return ImGuiKey_H;
		case Tridium::Input::KEY_I: return ImGuiKey_I;
		case Tridium::Input::KEY_J: return ImGuiKey_J;
		case Tridium::Input::KEY_K: return ImGuiKey_K;
		case Tridium::Input::KEY_L: return ImGuiKey_L;
		case Tridium::Input::KEY_M: return ImGuiKey_M;
		case Tridium::Input::KEY_N: return ImGuiKey_N;
		case Tridium::Input::KEY_O: return ImGuiKey_O;
		case Tridium::Input::KEY_P: return ImGuiKey_P;
		case Tridium::Input::KEY_Q: return ImGuiKey_Q;
		case Tridium::Input::KEY_R: return ImGuiKey_R;
		case Tridium::Input::KEY_S: return ImGuiKey_S;
		case Tridium::Input::KEY_T: return ImGuiKey_T;
		case Tridium::Input::KEY_U: return ImGuiKey_U;
		case Tridium::Input::KEY_V: return ImGuiKey_V;
		case Tridium::Input::KEY_W: return ImGuiKey_W;
		case Tridium::Input::KEY_X: return ImGuiKey_X;
		case Tridium::Input::KEY_Y: return ImGuiKey_Y;
		case Tridium::Input::KEY_Z: return ImGuiKey_Z;
		case Tridium::Input::KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
		case Tridium::Input::KEY_BACKSLASH: return ImGuiKey_Backslash;
		case Tridium::Input::KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
		case Tridium::Input::KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;

		TODO( "Currently returns None for World Keys" );
		case Tridium::Input::KEY_WORLD_1: return ImGuiKey_None;
		case Tridium::Input::KEY_WORLD_2: return ImGuiKey_None;

		case Tridium::Input::KEY_ESCAPE: return ImGuiKey_Escape;
		case Tridium::Input::KEY_ENTER: return ImGuiKey_Enter;
		case Tridium::Input::KEY_TAB: return ImGuiKey_Tab;
		case Tridium::Input::KEY_BACKSPACE: return ImGuiKey_Backspace;
		case Tridium::Input::KEY_INSERT: return ImGuiKey_Insert;
		case Tridium::Input::KEY_DELETE: return ImGuiKey_Delete;
		case Tridium::Input::KEY_RIGHT: return ImGuiKey_RightArrow;
		case Tridium::Input::KEY_LEFT: return ImGuiKey_LeftArrow;
		case Tridium::Input::KEY_DOWN: return ImGuiKey_DownArrow;
		case Tridium::Input::KEY_UP: return ImGuiKey_UpArrow;
		case Tridium::Input::KEY_PAGE_UP: return ImGuiKey_PageUp;
		case Tridium::Input::KEY_PAGE_DOWN: return ImGuiKey_PageDown;
		case Tridium::Input::KEY_HOME: return ImGuiKey_Home;
		case Tridium::Input::KEY_END: return ImGuiKey_End;
		case Tridium::Input::KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
		case Tridium::Input::KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
		case Tridium::Input::KEY_NUM_LOCK: return ImGuiKey_NumLock;
		case Tridium::Input::KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
		case Tridium::Input::KEY_PAUSE: return ImGuiKey_Pause;
		case Tridium::Input::KEY_F1: return ImGuiKey_F1;
		case Tridium::Input::KEY_F2: return ImGuiKey_F2;
		case Tridium::Input::KEY_F3: return ImGuiKey_F3;
		case Tridium::Input::KEY_F4: return ImGuiKey_F4;
		case Tridium::Input::KEY_F5: return ImGuiKey_F5;
		case Tridium::Input::KEY_F6: return ImGuiKey_F6;
		case Tridium::Input::KEY_F7: return ImGuiKey_F7;
		case Tridium::Input::KEY_F8: return ImGuiKey_F8;
		case Tridium::Input::KEY_F9: return ImGuiKey_F9;
		case Tridium::Input::KEY_F10: return ImGuiKey_F10;
		case Tridium::Input::KEY_F11: return ImGuiKey_F11;
		case Tridium::Input::KEY_F12: return ImGuiKey_F12;

		/* Imgui does not have key codes for F13 to F25*/
		case Tridium::Input::KEY_F13: return ImGuiKey_None;
		case Tridium::Input::KEY_F14: return ImGuiKey_None;
		case Tridium::Input::KEY_F15: return ImGuiKey_None;
		case Tridium::Input::KEY_F16: return ImGuiKey_None;
		case Tridium::Input::KEY_F17: return ImGuiKey_None;
		case Tridium::Input::KEY_F18: return ImGuiKey_None;
		case Tridium::Input::KEY_F19: return ImGuiKey_None;
		case Tridium::Input::KEY_F20: return ImGuiKey_None;
		case Tridium::Input::KEY_F21: return ImGuiKey_None;
		case Tridium::Input::KEY_F22: return ImGuiKey_None;
		case Tridium::Input::KEY_F23: return ImGuiKey_None;
		case Tridium::Input::KEY_F24: return ImGuiKey_None;
		case Tridium::Input::KEY_F25: return ImGuiKey_None;

		case Tridium::Input::KEY_KP_0: return ImGuiKey_Keypad0;
		case Tridium::Input::KEY_KP_1: return ImGuiKey_Keypad1;
		case Tridium::Input::KEY_KP_2: return ImGuiKey_Keypad2;
		case Tridium::Input::KEY_KP_3: return ImGuiKey_Keypad3;
		case Tridium::Input::KEY_KP_4: return ImGuiKey_Keypad4;
		case Tridium::Input::KEY_KP_5: return ImGuiKey_Keypad5;
		case Tridium::Input::KEY_KP_6: return ImGuiKey_Keypad6;
		case Tridium::Input::KEY_KP_7: return ImGuiKey_Keypad7;
		case Tridium::Input::KEY_KP_8: return ImGuiKey_Keypad8;
		case Tridium::Input::KEY_KP_9: return ImGuiKey_Keypad9;
		case Tridium::Input::KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
		case Tridium::Input::KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
		case Tridium::Input::KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
		case Tridium::Input::KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
		case Tridium::Input::KEY_KP_ADD: return ImGuiKey_KeypadAdd;
		case Tridium::Input::KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
		case Tridium::Input::KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
		case Tridium::Input::KEY_LEFT_SHIFT:return ImGuiKey_LeftShift;
		case Tridium::Input::KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
		case Tridium::Input::KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
		case Tridium::Input::KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
		case Tridium::Input::KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
		case Tridium::Input::KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
		case Tridium::Input::KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
		case Tridium::Input::KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
		case Tridium::Input::KEY_MENU: return ImGuiKey_Menu;

		default: return ImGuiKey_None;
		}
	}
}
