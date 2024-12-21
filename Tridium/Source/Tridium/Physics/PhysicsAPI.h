#pragma once

namespace Tridium {

	enum class EPhysicsAPI
	{
		None = 0,
		Jolt
	};

	static constexpr EPhysicsAPI s_PhysicsAPI = EPhysicsAPI::Jolt;

}