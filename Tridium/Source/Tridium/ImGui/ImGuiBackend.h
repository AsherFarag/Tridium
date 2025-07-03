#pragma once
#include "ImGui.h"
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Core/Memory.h>

namespace Tridium {

	enum class EImGuiPlatform
	{
		Unknown = 0,
		GLFW,
	};

	enum class EImGuiRenderer
	{
		Unknown = 0,
		OpenGL,
		Vulkan,
		DX11,
		DX12,
	};

	class IPlatformImGuiInterface
	{
	public:
		static UniquePtr<IPlatformImGuiInterface> Create( EImGuiPlatform a_Platform, EImGuiRenderer a_Renderer );
		virtual bool Init( void* a_Data, bool a_InstallCallbacks, EImGuiRenderer a_Renderer ) = 0;
		virtual void Shutdown() = 0;
		virtual void NewFrame() = 0;
		virtual void InstallCallbacks() = 0;
		virtual void RestoreCallbacks() = 0;
	};

	class IRendererImGuiInterface
	{
	public:
		static UniquePtr<IRendererImGuiInterface> Create( EImGuiRenderer a_Renderer );
		virtual bool Init( class IDynamicRHI* a_RHI ) = 0;
		virtual void Shutdown() = 0;
		virtual void NewFrame() = 0;
		virtual void RenderDrawData( ImDrawData* a_DrawData, const RHICommandListRef& a_CmdList, const RHITextureRef& a_RenderTarget ) = 0;
	};

}