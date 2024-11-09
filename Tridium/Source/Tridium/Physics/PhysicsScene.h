#pragma once
#include <Tridium/Core/Memory.h>

namespace Tridium {

	using PhysicsBodyID = uint32_t;

	class PhysicsScene
	{
	public:
		static UniquePtr<PhysicsScene> Create();

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void Tick( float a_TimeStep ) = 0;

		virtual PhysicsBodyID AddPhysicsBody(void* a_CreationSettings) = 0;
	};

} // namespace Tridium