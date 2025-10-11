#include "tripch.h"
#include "RendererModule.h"
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/Renderer/ShaderLibrary.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>

namespace Tridium {

	REGISTER_TICK_GROUP( BeginRender );
    REGISTER_TICK_GROUP( EndRender, "BeginRender"_H );

	REGISTER_ENGINE_MODULE( RendererModule );

    void RendererModule::Init()
    {
		// Initialise the Dynamic RHI
        const auto config = RHIConfig{}
            .SetRHIType( ERHInterfaceType::DirectX12 )
            .SetUseDebug( true )
            .SetSwapChainDesc(
                RHISwapChainDesc{}
                .SetWidth( 1280 )
                .SetHeight( 720 )
                .SetBufferCount( 2 )
                .SetFormat( ERHIFormat::RGBA8_UNORM )
                .SetFlags( ERHISwapChainFlags::UseVSync )
                .SetName( "Main SwapChain" )
            );

        if ( !ASSERT( RHI::Initialise( config ), "Failed to initialise the RHI" ) )
        {
            return;
        }

        m_DynamicRHI = RHI::GetDynamicRHI();

        RenderResourceManager::Init();
		ShaderLibrary::Init();

		//Application::AddOnTick( TickGroups::BeginRender, []() { RendererModule::Get()->BeginFrame(); } );
		//Application::AddOnTick( TickGroups::EndRender, []() { RendererModule::Get()->EndFrame(); } );
    }

    void RendererModule::Shutdown()
    {
        ShaderLibrary::Shutdown();
		RenderResourceManager::Shutdown();

        // Shutdown the Dynamic RHI
        if ( !ASSERT( RHI::Shutdown(), "Failed to shutdown the RHI" ) )
        {
            m_DynamicRHI = nullptr;
            return;
        }

    }

    void RendererModule::BeginFrame()
    {
        m_DynamicRHI->BeginFrame();
        m_PipelineManager.BeginFrame();
    }

    void RendererModule::EndFrame()
    {
        m_PipelineManager.EndFrame();
		m_DynamicRHI->EndFrame();

        m_FrameIndex = ( m_FrameIndex + 1 ) % RHI::GetDynamicRHI()->MaxFramesInFlight();
	}

} // namespace Tridium
