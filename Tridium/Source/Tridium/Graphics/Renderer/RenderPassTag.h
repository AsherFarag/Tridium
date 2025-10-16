#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Core/Handle.h>
#include <Tridium/Containers/BitSet.h>

namespace Tridium {

	//=============================================================================================
	// Render Pass Tag: Used to categorize render passes and control which passes are executed.
	//=============================================================================================
	using RenderPassTag = Handle<uint64_t, 0>;

	//=============================================================================================
	// Maximum number of distinct render pass tags supported.
	//=============================================================================================
	static constexpr size_t RenderPassTagCount = sizeof( RenderPassTag ) * 8;
	
	//=============================================================================================
	// Render Pass Tag Mask: A bitmask representing a set of render pass tags.
	//=============================================================================================
	using RenderPassTagMask = BitSet<RenderPassTagCount>;

	//=============================================================================================
	// Built-in Render Pass Tags:
	//=============================================================================================
	namespace RenderPassTags {

		static constexpr RenderPassTag Opaque			= RenderPassTag( 1 << 0 ); // Standard opaque geometry rendering
		static constexpr RenderPassTag Transparent		= RenderPassTag( 1 << 1 ); // Transparent geometry rendering
		static constexpr RenderPassTag Shadow			= RenderPassTag( 1 << 2 ); // Shadow map generation
		static constexpr RenderPassTag Skybox			= RenderPassTag( 1 << 3 ); // Skybox rendering
		static constexpr RenderPassTag UI				= RenderPassTag( 1 << 4 ); // User interface rendering

		//=============================================================================================
		// The total number of built-in render pass tags.
		// All user-defined tags must use bits >= this value.
		// e.g. RenderPassTag MyCustomPass1 = RenderPassTag( 1 << RenderPassTags::BuiltInCount );
		//      RenderPassTag MyCustomPass2 = RenderPassTag( 1 << RenderPassTags::BuiltInCount + 1 );
		static constexpr size_t BuiltInCount = 5;

	} // namespace RenderPassTags

} // namespace Tridium