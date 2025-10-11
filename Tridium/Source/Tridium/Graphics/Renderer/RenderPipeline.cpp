#include "tripch.h"
#include "RenderPipeline.h"
#include <Tridium/Graphics/Renderer/RendererModule.h>

namespace Tridium {

	IRenderPipeline* RenderPipelineManager::GetRenderPipeline( uint32_t a_FrameIndex )
	{
		if ( a_FrameIndex == ~0u )
		{
			a_FrameIndex = RendererModule::GetFrameIndex();
		}

		if ( !ASSERT( a_FrameIndex < m_RenderPipelines.Size(), "Frame index {} is out of bounds (max {})",
					  a_FrameIndex, m_RenderPipelines.Size() - 1 ) )
		{
			return nullptr;
		}

		return m_RenderPipelines.At( a_FrameIndex ).Pipeline.get();
	}

	void RenderPipelineManager::AddView( const RenderView& a_View )
	{
		// We create the constant buffers later at render time, so just add a null ref here.
		m_ViewsNextFrame.EmplaceBack( a_View, nullptr );
	}

	void RenderPipelineManager::BeginFrame()
	{
		// We are now rendering the views that between last frames 'BeginFrame' and this frames 'BeginFrame'.
		std::swap( m_Views, m_ViewsNextFrame );
		m_ViewsNextFrame.Clear();
	}

	void RenderPipelineManager::EndFrame()
	{
	}

} // namespace Tridium