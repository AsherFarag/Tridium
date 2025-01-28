#include "tripch.h"
#include "RHICommon.h"
#include "RenderContext.h"

// Backends
#include "Backend/DirectX12/DX12RendererAPI.h"
#include "Backend/OpenGL/OpenGLRendererAPI.h"

namespace Tridium {


	bool RHI::Initialise( const RHIConfig& a_Config )
	{
		if ( !ASSERT_LOG( RenderContext::Get() == nullptr, "Render Context has already been previously initialised!" ) )
		{
			return false;
		}

		if ( !ASSERT_LOG( RHI::IsGraphicsAPISupported( a_Config.GraphicsAPI ), "Graphics API is not supported!" ) )
		{
			return false;
		}

		RenderContext::s_Instance = new RenderContext();
		RenderContext::Get()->m_Config = a_Config;

		switch ( a_Config.GraphicsAPI )
		{
			case EGraphicsAPI::OpenGL:
			{
				RenderContext::Get()->m_RendererAPI = MakeUnique<OpenGLRendererAPI>();
				break;
			}
			case EGraphicsAPI::DirectX12:
			{
				RenderContext::Get()->m_RendererAPI = MakeUnique<DX12RendererAPI>();
				break;
			}
			default:
			{
				ASSERT( false );
				return false;
			}
		}

		bool success = RenderContext::Get()->m_RendererAPI->Init( a_Config );
		return success;
	}

	bool RHI::Shutdown()
	{
		if ( RenderContext::s_Instance == nullptr )
		{
			return false;
		}

		// Shutdown the rendering API
		bool success = RenderContext::Get()->m_RendererAPI->Shutdown();

		delete RenderContext::s_Instance;
		RenderContext::s_Instance = nullptr;
		return success;
	}

	bool RHI::Present()
	{
		if ( !ASSERT_LOG( RenderContext::Get() != nullptr, "Render Context has not been initialised!" ) )
		{
			return false;
		}

		return RenderContext::Get()->m_RendererAPI->Present();
	}

	const char* RHI::GetGraphicsAPIName( EGraphicsAPI a_API )
	{
		if ( RenderContext::Get() == nullptr && a_API != EGraphicsAPI::None )
		{
			return "None";
		}

		EGraphicsAPI api = a_API == EGraphicsAPI::None ? RenderContext::Get()->GetGraphicsAPI() : a_API;
		switch ( api )
		{
			case EGraphicsAPI::OpenGL: return "OpenGL";
			case EGraphicsAPI::DirectX11: return "DirectX 11";
			case EGraphicsAPI::DirectX12: return "DirectX 12";
			case EGraphicsAPI::Vulkan: return "Vulkan";
		}

		return "None";
	}

	constexpr bool RHI::IsGraphicsAPISupported( EGraphicsAPI a_API )
	{
		switch ( a_API )
		{
			case EGraphicsAPI::OpenGL: return true;
			case EGraphicsAPI::DirectX11: return false;
			case EGraphicsAPI::DirectX12: return true;
			case EGraphicsAPI::Vulkan: return false;
		}

		return false;
	}

}