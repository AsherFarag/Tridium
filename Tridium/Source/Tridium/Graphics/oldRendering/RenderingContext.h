#pragma once

struct GLFWwindow;

namespace Tridium {
	class RenderingContext
	{
	public:
		TODO( "Add a way to create a rendering context for different APIs" );
		static UniquePtr<RenderingContext> Create( GLFWwindow* window );
		virtual bool Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}