#include "tripch.h"
#include "RHI.h"

// Backends
#if RHI_ENABLE_BACKEND_DIRECTX12
	#include "Backend/DirectX12/DX12RHI.h"
#endif
#if RHI_ENABLE_BACKEND_OPENGL
	#include "Backend/OpenGL/OpenGLRHI.h"
#endif

namespace Tridium {

	DynamicRHI* s_DynamicRHI = nullptr;

	struct Vertex
	{

	};

	bool RHI::Initialise( const RHIConfig& a_Config )
	{
		constexpr RHIVertexLayout layout = {
			{ "Position", RHIVertexElementTypes::Float3 },
			{ "Normal", RHIVertexElementTypes::Float3 },
			{ "Tangent", RHIVertexElementTypes::Float3 },
			{ "TexCoord", RHIVertexElementTypes::Float2 },
			{ "Colour", RHIVertexElementTypes::Float4 }
		};

		if ( !ASSERT_LOG( !s_RHIGlobals.IsRHIInitialised, "RHI has already been previously initialised!" ) )
		{
			return false;
		}

		switch ( a_Config.RHIType )
		{
		#if RHI_ENABLE_BACKEND_OPENGL
			case ERHInterfaceType::OpenGL:
			{
				s_DynamicRHI = new OpenGLRHI();
				break;
			}
		#endif
		#if RHI_ENABLE_BACKEND_DIRECTX12
			case ERHInterfaceType::DirectX12:
			{
				s_DynamicRHI = new DirectX12RHI();
				break;
			}
		#endif
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
