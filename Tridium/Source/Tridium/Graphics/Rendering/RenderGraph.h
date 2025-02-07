#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Graphics/Rendering/RenderPass/RenderPass.h>

namespace Tridium {

	class RenderGraph
	{
	public:
		RenderGraph() = default;
		~RenderGraph() = default;

		template<typename T, typename... _Args>
		RenderGraph& AddPass( _Args&&... args )
		{
			static_assert( std::is_base_of<IRenderPass, T>::value, "RenderGraph::AddPass: T must derive from IRenderPass" );
			m_Passes.push_back( std::make_unique<T>( std::forward<_Args>( args )... ) );
			return *this;
		}

	private:
		std::vector<UniquePtr<IRenderPass>> m_Passes;
	};

}