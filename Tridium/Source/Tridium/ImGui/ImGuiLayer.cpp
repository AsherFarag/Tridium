#include "tripch.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Engine/Engine.h>
#include <Tridium/ImGui/ImGuiLayer.h>
#include <Tridium/ImGui/ImGuiModule.h>
#include <Tridium/ImGui/Backends/ImGuiBackend_RHI.h>
#include <Tridium/Input/Input.h>

#include <GLFW/glfw3.h>

namespace Tridium {

	static void AddIconsToFont()
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
		// - Set Default Font -
		ImGui::GetLightFont();
		AddIconsToFont();
		ImGui::GetIO().FontDefault = ImGui::GetRegularFont();
		AddIconsToFont();
		ImGui::GetBoldFont();
		AddIconsToFont();
		ImGui::GetExtraBoldFont();
		AddIconsToFont();
	}

	void ImGuiLayer::OnDetach()
	{

	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplRHI_NewFrame();
		ImGuiModule::GetPlatformBackend()->NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		// Size Initialization
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2( Application::Get()->GetWindow().GetWidth(), Application::Get()->GetWindow().GetHeight() );


		ImGui::Render();

		IRHISwapChain* swapChain = RHI::GetSwapChain();
		if ( !swapChain )
		{
			ENSURE( false, "Swap chain is null!" );
			return;
		}

		// Set up the command list for rendering ImGui

		ImGui_ImplRHI_RenderDrawData( ImGui::GetDrawData(), RHI::GetDynamicRHI(), swapChain->GetBackBuffer() );

		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			GLFWwindow* backUpCurrentContext = glfwGetCurrentContext();
			//m_PlatformBackend->RestoreCallbacks();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent( backUpCurrentContext );
		}
	}

	void ImGuiLayer::OnImGuiDraw()
	{
	}

	ImGuiKey ImGuiLayer::TridiumToImGuiKey( EInputKey key )
	{
		switch ( key )
		{
		case EInputKey::InvalidKey: return ImGuiKey_None;
		case EInputKey::Space: return ImGuiKey_Space;
		case EInputKey::Apostrophe: return ImGuiKey_Apostrophe;
		case EInputKey::Comma: return ImGuiKey_Comma;
		case EInputKey::Minus: return ImGuiKey_Minus;
		case EInputKey::Period: return ImGuiKey_Period;
		case EInputKey::Slash: return ImGuiKey_Slash;
		case EInputKey::Zero: return ImGuiKey_0;
		case EInputKey::One: return ImGuiKey_1;
		case EInputKey::Two: return ImGuiKey_2;
		case EInputKey::Three: return ImGuiKey_3;
		case EInputKey::Four: return ImGuiKey_4;
		case EInputKey::Five: return ImGuiKey_5;
		case EInputKey::Six: return ImGuiKey_6;
		case EInputKey::Seven: return ImGuiKey_7;
		case EInputKey::Eight: return ImGuiKey_8;
		case EInputKey::Nine: return ImGuiKey_9;
		case EInputKey::Semicolon: return ImGuiKey_Semicolon;
		case EInputKey::Equal: return ImGuiKey_Equal;
		case EInputKey::A: return ImGuiKey_A;
		case EInputKey::B: return ImGuiKey_B;
		case EInputKey::C: return ImGuiKey_C;
		case EInputKey::D: return ImGuiKey_D;
		case EInputKey::E: return ImGuiKey_E;
		case EInputKey::F: return ImGuiKey_F;
		case EInputKey::G: return ImGuiKey_G;
		case EInputKey::H: return ImGuiKey_H;
		case EInputKey::I: return ImGuiKey_I;
		case EInputKey::J: return ImGuiKey_J;
		case EInputKey::K: return ImGuiKey_K;
		case EInputKey::L: return ImGuiKey_L;
		case EInputKey::M: return ImGuiKey_M;
		case EInputKey::N: return ImGuiKey_N;
		case EInputKey::O: return ImGuiKey_O;
		case EInputKey::P: return ImGuiKey_P;
		case EInputKey::Q: return ImGuiKey_Q;
		case EInputKey::R: return ImGuiKey_R;
		case EInputKey::S: return ImGuiKey_S;
		case EInputKey::T: return ImGuiKey_T;
		case EInputKey::U: return ImGuiKey_U;
		case EInputKey::V: return ImGuiKey_V;
		case EInputKey::W: return ImGuiKey_W;
		case EInputKey::X: return ImGuiKey_X;
		case EInputKey::Y: return ImGuiKey_Y;
		case EInputKey::Z: return ImGuiKey_Z;
		case EInputKey::LeftBracket: return ImGuiKey_LeftBracket;
		case EInputKey::Backslash: return ImGuiKey_Backslash;
		case EInputKey::RightBracket: return ImGuiKey_RightBracket;
		case EInputKey::GraveAccent: return ImGuiKey_GraveAccent;

		TODO( "Currently returns None for World Keys" );
		case EInputKey::World1: return ImGuiKey_None;
		case EInputKey::World2: return ImGuiKey_None;

		case EInputKey::Escape: return ImGuiKey_Escape;
		case EInputKey::Enter: return ImGuiKey_Enter;
		case EInputKey::Tab: return ImGuiKey_Tab;
		case EInputKey::Backspace: return ImGuiKey_Backspace;
		case EInputKey::Insert: return ImGuiKey_Insert;
		case EInputKey::Delete: return ImGuiKey_Delete;
		case EInputKey::Right: return ImGuiKey_RightArrow;
		case EInputKey::Left: return ImGuiKey_LeftArrow;
		case EInputKey::Down: return ImGuiKey_DownArrow;
		case EInputKey::Up: return ImGuiKey_UpArrow;
		case EInputKey::PageUp: return ImGuiKey_PageUp;
		case EInputKey::PageDown: return ImGuiKey_PageDown;
		case EInputKey::Home: return ImGuiKey_Home;
		case EInputKey::End: return ImGuiKey_End;
		case EInputKey::CapsLock: return ImGuiKey_CapsLock;
		case EInputKey::ScrollLock: return ImGuiKey_ScrollLock;
		case EInputKey::NumLock: return ImGuiKey_NumLock;
		case EInputKey::PrintScreen: return ImGuiKey_PrintScreen;
		case EInputKey::Pause: return ImGuiKey_Pause;
		case EInputKey::F1: return ImGuiKey_F1;
		case EInputKey::F2: return ImGuiKey_F2;
		case EInputKey::F3: return ImGuiKey_F3;
		case EInputKey::F4: return ImGuiKey_F4;
		case EInputKey::F5: return ImGuiKey_F5;
		case EInputKey::F6: return ImGuiKey_F6;
		case EInputKey::F7: return ImGuiKey_F7;
		case EInputKey::F8: return ImGuiKey_F8;
		case EInputKey::F9: return ImGuiKey_F9;
		case EInputKey::F10: return ImGuiKey_F10;
		case EInputKey::F11: return ImGuiKey_F11;
		case EInputKey::F12: return ImGuiKey_F12;

		/* Imgui does not have key codes for F13 to F25*/
		case EInputKey::F13: return ImGuiKey_None;
		case EInputKey::F14: return ImGuiKey_None;
		case EInputKey::F15: return ImGuiKey_None;
		case EInputKey::F16: return ImGuiKey_None;
		case EInputKey::F17: return ImGuiKey_None;
		case EInputKey::F18: return ImGuiKey_None;
		case EInputKey::F19: return ImGuiKey_None;
		case EInputKey::F20: return ImGuiKey_None;
		case EInputKey::F21: return ImGuiKey_None;
		case EInputKey::F22: return ImGuiKey_None;
		case EInputKey::F23: return ImGuiKey_None;
		case EInputKey::F24: return ImGuiKey_None;
		case EInputKey::F25: return ImGuiKey_None;

		case EInputKey::KeyPad0: return ImGuiKey_Keypad0;
		case EInputKey::KeyPad1: return ImGuiKey_Keypad1;
		case EInputKey::KeyPad2: return ImGuiKey_Keypad2;
		case EInputKey::KeyPad3: return ImGuiKey_Keypad3;
		case EInputKey::KeyPad4: return ImGuiKey_Keypad4;
		case EInputKey::KeyPad5: return ImGuiKey_Keypad5;
		case EInputKey::KeyPad6: return ImGuiKey_Keypad6;
		case EInputKey::KeyPad7: return ImGuiKey_Keypad7;
		case EInputKey::KeyPad8: return ImGuiKey_Keypad8;
		case EInputKey::KeyPad9: return ImGuiKey_Keypad9;
		case EInputKey::KeyPadDecimal: return ImGuiKey_KeypadDecimal;
		case EInputKey::KeyPadDivide: return ImGuiKey_KeypadDivide;
		case EInputKey::KeyPadMultiply: return ImGuiKey_KeypadMultiply;
		case EInputKey::KeyPadSubtract: return ImGuiKey_KeypadSubtract;
		case EInputKey::KeyPadAdd: return ImGuiKey_KeypadAdd;
		case EInputKey::KeyPadEnter: return ImGuiKey_KeypadEnter;
		case EInputKey::KeyPadEqual: return ImGuiKey_KeypadEqual;
		case EInputKey::LeftShift:return ImGuiKey_LeftShift;
		case EInputKey::LeftControl: return ImGuiKey_LeftCtrl;
		case EInputKey::LeftAlt: return ImGuiKey_LeftAlt;
		case EInputKey::LeftSuper: return ImGuiKey_LeftSuper;
		case EInputKey::RightShift: return ImGuiKey_RightShift;
		case EInputKey::RightControl: return ImGuiKey_RightCtrl;
		case EInputKey::RightAlt: return ImGuiKey_RightAlt;
		case EInputKey::RightSuper: return ImGuiKey_RightSuper;
		case EInputKey::Menu: return ImGuiKey_Menu;

		default: return ImGuiKey_None;
		}
	}
}