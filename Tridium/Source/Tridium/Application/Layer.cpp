#include "tripch.h"
#include "Layer.h"

namespace Tridium {

	void AppLayerStack::PushLayer( SharedPtr<IAppLayer> a_Layer )
	{
		if ( !a_Layer )
		{
			ASSERT( false, "Trying to push a null layer to the layer stack!" );
			return;
		}

		auto it = m_Layers.Emplace( m_Layers.Begin() + m_LayerInsertIndex++, std::move( a_Layer ) );
		( *it )->OnAttach();
	}

	void AppLayerStack::PushOverlay( SharedPtr<IAppLayer> a_Overlay )
	{
		if ( !a_Overlay )
		{
			ASSERT( false, "Trying to push a null overlay to the layer stack!" );
			return;
		}

		m_Layers.EmplaceBack( std::move( a_Overlay ) )->OnAttach();
	}

	void AppLayerStack::PopLayer( const SharedPtr<IAppLayer>& a_Layer )
	{
		auto it = std::find( m_Layers.Begin(), m_Layers.End(), a_Layer );

		if ( it != m_Layers.End() )
		{
			( *it )->OnDetach();
			m_Layers.Erase( it );
			m_LayerInsertIndex--;
		}
	}

	void AppLayerStack::PopOverlay( const SharedPtr<IAppLayer>& a_Overlay )
	{
		auto it = std::find( m_Layers.Begin(), m_Layers.End(), a_Overlay );

		if ( it != m_Layers.End() )
		{
			( *it )->OnDetach();
			m_Layers.Erase( it );
		}
	}

	void AppLayerStack::OnUpdate( float a_DeltaTime )
	{
		// Update layers from top to bottom
		for ( auto it = m_Layers.RBegin(); it != m_Layers.REnd(); ++it )
		{
			( *it )->OnUpdate();
		}
	}

	void AppLayerStack::OnEvent( Event& a_Event )
	{
		// Propagate events from top to bottom
		for ( auto it = m_Layers.RBegin(); it != m_Layers.REnd(); ++it )
		{
			( *it )->OnEvent( a_Event );
			if ( a_Event.Handled )
				break;
		}
	}

} // namespace Tridium