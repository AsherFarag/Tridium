#include "tripch.h"
#include "WindowsInput.h"

#include <Tridium/Core/Application.h>
#include <GLFW/glfw3.h>

TODO( "TEMP?! " );
#include <imgui.h>

namespace Tridium {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl( int keycode )
	{
		auto window = static_cast<GLFWwindow*>( Application::Get()->GetWindow().GetNativeWindow() );
		auto state = glfwGetKey( window, keycode );
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl( int button )
	{
		auto window = static_cast<GLFWwindow*>( Application::Get()->GetWindow().GetNativeWindow() );
		auto state = glfwGetMouseButton( window, button );
		return state == GLFW_PRESS;
	}

	Vector2 WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>( Application::Get()->GetWindow().GetNativeWindow() );
		double xpos, ypos;
		glfwGetCursorPos( window, &xpos, &ypos );
		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		return GetMousePositionImpl().x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		return GetMousePositionImpl().y;
	}

	void WindowsInput::SetInputModeImpl( EInputMode a_Mode, EInputModeValue a_Value )
	{
		int mode;
		switch ( a_Mode )
		{
		case EInputMode::Cursor:
			mode = GLFW_CURSOR;
			break;
		case EInputMode::Sticky_Keys:
			mode = GLFW_STICKY_KEYS;
			break;
		case EInputMode::Sticky_Mouse_Buttons:
			mode = GLFW_STICKY_MOUSE_BUTTONS;
			break;
		case EInputMode::Lock_Key_Mods:
			mode = GLFW_LOCK_KEY_MODS;
			break;
		case EInputMode::Raw_Mouse_Motion:
			mode = GLFW_RAW_MOUSE_MOTION;
			break;
		case EInputMode::Unlimited_Mouse_Buttons:
			mode = GLFW_UNLIMITED_MOUSE_BUTTONS;
			break;
		default:
			mode = GLFW_INVALID_ENUM;
			break;
		}

		int value;
		switch ( a_Value )
		{
		case EInputModeValue::False:
		{
			value = GLFW_FALSE;
			break;
		}
		case EInputModeValue::True:
		{
			value = GLFW_TRUE;
			break;
		}
		case EInputModeValue::Cursor_Normal:
		{
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			value = GLFW_CURSOR_NORMAL;
			break;
		}
		case EInputModeValue::Cursor_Hidden:
		{
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			value = GLFW_CURSOR_HIDDEN;
			break;
		}
		case EInputModeValue::Cursor_Disabled:
		{
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			value = GLFW_CURSOR_DISABLED;
			break;
		}
		case EInputModeValue::Cursor_Captured:
		{
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			value = GLFW_CURSOR_CAPTURED;
			break;
		}
		default:
		{
			value = GLFW_INVALID_ENUM;
			break;
		}
		}

		auto window = static_cast<GLFWwindow*>( Application::Get()->GetWindow().GetNativeWindow() );
		glfwSetInputMode( window, mode, value );
	}

}