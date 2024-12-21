#pragma once
#include <Tridium/Physics/PhysicsEngine.h>

namespace Tridium {

	class JoltPhysicsEngine final : public PhysicsEngine
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
	};

} // namespace Tridium