#pragma once
#include <Tridium/Graphics/RHI/RHICommon.h>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

namespace Tridium {

	// Static API wrapper for OpenGL functions
	// Newer versions can inherit from this class and override the static functions
	class OpenGLAPI
	{
	public:
		enum class EResourceLockMode : uint8_t
		{
			ReadOnly,
			WriteOnly,
			ReadWrite,
			WriteOnlyUnsynchronized,
			ReadOnlyPersistent,
			WriteOnlyPersistent,
		};

		enum class EQueryMode : uint8_t
		{
			Result,
			ResultAvailable,
		};

		enum class EFenceResult : uint8_t
		{
			AlreadySignaled,
			TimeoutExpired,
			ConditionSatisfied,
			WaitFailed,
		};
	};

} // namespace Tridium
