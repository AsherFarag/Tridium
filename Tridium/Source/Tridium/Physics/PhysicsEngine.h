#pragma once
#include "PhysicsAPI.h"
#include <Tridium/Core/Memory.h>

namespace Tridium
{
	class PhysicsEngine
	{
	public:
		static UniquePtr<PhysicsEngine> Create();

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
	};

}