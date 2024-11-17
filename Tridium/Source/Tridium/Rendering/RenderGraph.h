#pragma once
#include <Tridium/Core/Core.h>
#include "VertexArray.h"

namespace Tridium {

	class RenderGraph
	{
	public:
		RenderGraph() = default;
		~RenderGraph() = default;

		RenderGraph& AddRenderPass( const std::string& a_Name, const std::function<void()>& a_RenderPass )
		{
			return *this;
		}

	private:
	};

}