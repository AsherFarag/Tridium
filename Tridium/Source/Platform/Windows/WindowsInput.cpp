#include "tripch.h"
#include "WindowsInput.h"

#include <Tridium/Core/Application.h>
#include <GLFW/glfw3.h>

namespace Tridium {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl( int keycode )
	{
		auto window = static_cast<GLFWwindow*>( Application::Get().GetWindow().GetNativeWindow() );
		auto state = glfwGetKey( window, keycode );
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl( int button )
	{
		auto window = static_cast<GLFWwindow*>( Application::Get().GetWindow().GetNativeWindow() );
		auto state = glfwGetMouseButton( window, button );
		return state == GLFW_PRESS;
	}

	Vector2 WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>( Application::Get().GetWindow().GetNativeWindow() );
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

}