#pragma once

namespace Tridium {

	enum class EEngineInitStage
	{
		// Pre Engine Init is the first stage of engine initialization.
		// Called at the start of the engine initialization process.
		PreEngineInit,
		// Post Engine Init is the final stage of engine initialization.
		// Called at the end of the engine initialization process.
		PostEngineInit,

		Default = PostEngineInit,
	};

}