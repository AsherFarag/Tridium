#pragma once
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Common/Function.h>

namespace Tridium {

	struct RenderPass
	{
		String Name;
		SmallArray<RHIResourceRef, 8> Reads;
		SmallArray<RHIResourceRef, 8> Writes;
		Delegate<void( IRHICommandList& )> Execute;
	};

	class RenderGraph
	{
	public:

	};

}