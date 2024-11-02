#include "tripch.h"
#include "LayerStack.h"

namespace Tridium {

	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		for ( Layer* layer : m_Layers )
			delete layer;
	}

	void LayerStack::PushLayer( Layer* a_Layer )
	{
		m_Layers.emplace( m_Layers.begin() + m_LayerInsertIndex, a_Layer );
		m_LayerInsertIndex++;
		a_Layer->OnAttach();
	}

	void LayerStack::PushOverlay( Layer* a_Overlay )
	{
		m_Layers.emplace_back( a_Overlay );
		a_Overlay->OnAttach();
	}

	void LayerStack::PopLayer( Layer* a_Layer, bool a_Destroy )
	{
		auto it = std::find( m_Layers.begin(), m_Layers.end(), a_Layer );
		if ( it != m_Layers.end() )
		{
			( *it )->OnDetach();
			m_Layers.erase( it );
			m_LayerInsertIndex--;

			if ( a_Destroy )
				delete a_Layer;
		}
	}

	void LayerStack::PopOverlay( Layer* a_Overlay, bool a_Destroy )
	{
		auto it = std::find( m_Layers.begin(), m_Layers.end(), a_Overlay );
		if ( it != m_Layers.end() )
		{
			( *it )->OnDetach();
			m_Layers.erase( it );

			if ( a_Destroy )
				delete a_Overlay;
		}
	}

}