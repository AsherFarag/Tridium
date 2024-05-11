#pragma once

#include <Tridium/Core/Core.h>
#include <Tridium/Core/Layer.h>

namespace Tridium {

	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer( Layer* layer );
		void PushOverlay( Layer* overlay );
		void PopLayer( Layer* layer );
		void PopOverlay( Layer* overlay );

		auto begin() { return m_Layers.begin(); }
		auto end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};

}