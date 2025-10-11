#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Containers/BitSet.h>

namespace Tridium {

	//=============================================================================================
	// Maximum number of distinct render pass tags supported.
	//=============================================================================================
	static constexpr size_t RenderPassTagCount = 64u;

	//=============================================================================================
	// Render Pass Tag: Used to categorize render passes and control which passes are executed.
	//=============================================================================================
	enum class RenderPassTag : uint64_t {};

	//=============================================================================================
	// Render Pass Tag Mask: A bitset representing a combination of render pass tags.
	// Each bit corresponds to a specific RenderPassTag.
	//=============================================================================================
	using RenderPassTagMask = BitSet<RenderPassTagCount>;

	namespace RenderPassTags {

		static constexpr RenderPassTag Opaque      = (RenderPassTag)(1 << 0);
		static constexpr RenderPassTag Transparent = (RenderPassTag)(1 << 1);
		static constexpr RenderPassTag Shadow      = (RenderPassTag)(1 << 2);
		static constexpr RenderPassTag PostProcess = (RenderPassTag)(1 << 3);
		static constexpr RenderPassTag UI          = (RenderPassTag)(1 << 4);

	}

}