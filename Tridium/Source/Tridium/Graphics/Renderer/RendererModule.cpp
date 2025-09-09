#include "tripch.h"
#include "RendererModule.h"
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium {

	REGISTER_TICK_GROUP( BeginRender );
    REGISTER_TICK_GROUP( EndRender, "BeginRender"_H );

	REGISTER_ENGINE_MODULE( RendererModule );

    void RendererModule::Print()
    {
		printf( "Renderer Module:\n" );
	}

    void RendererModule::Init()
    {
		// Initialise the Dynamic RHI
        {
            RHIConfig config{};
            config.RHIType = ERHInterfaceType::DirectX12;
            config.UseDebug = true;
            config.SwapChainDesc = RHISwapChainDesc{}
                .SetWidth( 1280 )
                .SetHeight( 720 )
                .SetBufferCount( 2 )
                .SetFormat( ERHIFormat::RGBA8_UNORM )
                .SetFlags( ERHISwapChainFlags::UseVSync )
                .SetName( "Main SwapChain" );
            if ( !ASSERT( RHI::Initialise( config ),
                          "Failed to initialise the RHI" ) )
            {
                return;
			}

            m_DynamicRHI = RHI::GetDynamicRHI();
        }

        // Set up Shader Library Singleton
        ShaderLibrary::Singleton::BindExisting( &m_ShaderLibrary );

        // Set up Render Resource Manager Singleton
        RenderResourceManager::Singleton::BindExisting( &m_RenderResourceManager );
    }

    void RendererModule::Shutdown()
    {
        // Shutdown the Render Resource Manager
        RenderResourceManager::Singleton::Release();

        // Shutdown the Shader Library
        ShaderLibrary::Singleton::Release();

        // Shutdown the Dynamic RHI
        if ( !ASSERT( RHI::Shutdown(),
                      "Failed to shutdown the RHI" ) )
        {
            m_DynamicRHI = nullptr;
            return;
        }
    }

}
