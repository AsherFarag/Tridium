#include "tripch.h"
#include "RHI.h"

// Backends
#include "Backend/DirectX12/DX12RHI.h"
#include "Backend/OpenGL/OpenGLRHI.h"

namespace Tridium {

	DynamicRHI* s_DynamicRHI = nullptr;

	bool RHI::Initialise( const RHIConfig& a_Config )
	{
		if ( !ASSERT_LOG( !s_RHIGlobals.IsRHIInitialised, "RHI has already been previously initialised!" ) )
		{
			return false;
		}

		switch ( a_Config.RHIType )
		{
			case ERHInterfaceType::OpenGL:
			{
				s_DynamicRHI = new GL::OpenGLRHI();
				break;
			}
			case ERHInterfaceType::DirectX12:
			{
				s_DynamicRHI = new DX12::DirectX12RHI();
				break;
			}
			default:
			{
				ASSERT( false );
			}
		}

		if ( s_DynamicRHI == nullptr )
		{
			return false;
		}

		if ( s_DynamicRHI->Init( a_Config ) == false )
		{
			// Failed to initialise the rendering API
			delete s_DynamicRHI;
			s_DynamicRHI = nullptr;
			return false;
		}

		s_RHIGlobals.IsRHIInitialised = true;
		s_RHIGlobals.Config = a_Config;

		return true;
	}

	bool RHI::Shutdown()
	{
		if ( s_DynamicRHI == nullptr )
		{
			return false;
		}

		// Shutdown the rendering API
		bool success = s_DynamicRHI->Shutdown();

		delete s_DynamicRHI;
		s_DynamicRHI = nullptr;
		return success;
	}

	bool RHI::Present()
	{
		if ( !ASSERT_LOG( s_RHIGlobals.IsRHIInitialised == false, "RHI has not been initialised!" ) )
		{
			return false;
		}

		return s_DynamicRHI->Present();
	}

	ERHInterfaceType RHI::GetRHInterfaceType()
	{
		if ( s_DynamicRHI == nullptr )
		{
			CHECK( false );
			return ERHInterfaceType::Null;
		}

		return s_DynamicRHI->GetRHInterfaceType();
	}

	const char* RHI::GetRHIName( ERHInterfaceType a_API )
	{
		if ( s_DynamicRHI == nullptr && a_API == ERHInterfaceType::Null )
		{
			return "Null";
		}

		ERHInterfaceType api = a_API == ERHInterfaceType::Null ? s_DynamicRHI->GetRHInterfaceType() : a_API;
		switch ( api )
		{
			case ERHInterfaceType::OpenGL: return "OpenGL";
			case ERHInterfaceType::DirectX11: return "DirectX 11";
			case ERHInterfaceType::DirectX12: return "DirectX 12";
			case ERHInterfaceType::Vulkan: return "Vulkan";
		}

		return "Null";
	}
}
