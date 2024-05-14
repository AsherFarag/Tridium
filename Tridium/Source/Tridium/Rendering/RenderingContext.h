#pragma once

struct GLFWwindow;

namespace Tridium {
	class RenderingContext
	{
	public:
		virtual bool Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}