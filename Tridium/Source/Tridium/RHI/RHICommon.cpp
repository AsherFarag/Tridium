#include "tripch.h"
#include "RHICommon.h"
#include "RenderContext.h"

// Backends
#include "Backend/DirectX12/DX12RHI.h"
#include "Backend/OpenGL/OpenGLRHI.h"

namespace Tridium {


	bool RHI::Initialise( const RHIConfig& a_Config )
	{
		if ( !ASSERT_LOG( RenderContext::Get() == nullptr, "Render Context has already been previously initialised!" ) )
		{
			return false;
		}

		RenderContext::s_Instance = new RenderContext();
		RenderContext::Get()->m_Config = a_Config;

		switch ( a_Config.RHIType )
		{
			case ERHInterfaceType::OpenGL:
			{
				RenderContext::Get()->m_RHInterface = MakeUnique<GL::OpenGLRHI>();
				break;
			}
			case ERHInterfaceType::DirectX12:
			{
				RenderContext::Get()->m_RHInterface = MakeUnique<DX12::DirectX12RHI>();
				break;
			}
			default:
			{
				ASSERT( false );
			}
		}

		if ( RenderContext::Get()->m_RHInterface == nullptr )
		{
			delete RenderContext::s_Instance;
			return false;
		}

		if ( RenderContext::Get()->m_RHInterface->Init( a_Config ) == false )
		{
			// Failed to initialise the rendering API
			delete RenderContext::s_Instance;
			return false;
		}

		s_RHIGlobals.IsRHIInitialised = true;
		return true;
	}

	bool RHI::Shutdown()
	{
		if ( RenderContext::s_Instance == nullptr )
		{
			return false;
		}

		// Shutdown the rendering API
		bool success = RenderContext::Get()->m_RHInterface->Shutdown();

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

		return RenderContext::Get()->m_RHInterface->Present();
	}

	ERHInterfaceType RHI::GetRHInterfaceType()
	{
		if ( RenderContext::Get() == nullptr )
		{
			CHECK( false );
			return ERHInterfaceType::Null;
		}

		return RenderContext::Get()->GetRHInterfaceType();
	}

	const char* RHI::GetRHIName( ERHInterfaceType a_API )
	{
		if ( RenderContext::Get() == nullptr && a_API == ERHInterfaceType::Null )
		{
			return "Null";
		}

		ERHInterfaceType api = a_API == ERHInterfaceType::Null ? RenderContext::Get()->GetRHInterfaceType() : a_API;
		switch ( api )
		{
			case ERHInterfaceType::OpenGL: return "OpenGL";
			case ERHInterfaceType::DirectX11: return "DirectX 11";
			case ERHInterfaceType::DirectX12: return "DirectX 12";
			case ERHInterfaceType::Vulkan: return "Vulkan";
		}

		return "Null";
	}

	const RHIConfig& RHI::GetConfig()
	{
		if ( RenderContext::Get() == nullptr )
		{
			static RHIConfig s_InvalidConfig;
			return s_InvalidConfig;
		}

		return RenderContext::Get()->GetConfig();
	}
}
