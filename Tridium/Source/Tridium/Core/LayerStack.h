#pragma once

#include <Tridium/Core/Core.h>
#include <Tridium/Core/Layer.h>

namespace Tridium {

	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer( Layer* a_Layer );
		void PushOverlay( Layer* a_Overlay );
		void PopLayer( Layer* a_Layer, bool a_Destroy = false );
		void PopOverlay( Layer* a_Overlay, bool a_Destroy = false );

		inline auto begin() { return m_Layers.begin(); }
		inline auto end() { return m_Layers.end(); }
		inline auto rbegin() { return m_Layers.rbegin(); }
		inline auto rend() { return m_Layers.rend(); }

		inline size_t NumLayers() { return m_Layers.size(); }
		Layer* operator[]( unsigned int a_Index ) { return m_Layers[a_Index]; }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};

}